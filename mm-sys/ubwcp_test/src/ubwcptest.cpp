/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "UBWCPLib.h"
#include <log/log.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <BufferAllocator/BufferAllocatorWrapper.h>
#include <BufferAllocator/BufferAllocator.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdatomic.h>

#define UBWCP_INFO 	0
#define UBWCP_WARN 	1
#define UBWCP_ERR	2

int debug_level = UBWCP_ERR;
bool dump_diff_to_file = false;
bool pause_on_failure = false;

//#define UBWCP_LOG(level, ...) \
//            do { ALOGE(__VA_ARGS__); } while (0)

#define UBWCP_LOG(level, ...) \
            do { if (level >= debug_level) { printf(__VA_ARGS__); printf("\n");} } while (0)

//TODO: Change to use provided definition once available
#define DmabufUBWCPHeapName "qcom,ubwcp"


#define ALL_IDS -1
#define ALL_UBWCP_FORMATS -1

static void set_debug_level(int level)
{
	debug_level = level;
}

struct ubwcp_test;

typedef int (*test_fn)(ubwcp_test *test);

typedef struct Image_Attrs {
	int id;
	int w;
	int h;
	int stride;
	int scanlines;
	bool linear;
	UBWCPLib_Image_Format format;
	int planar_padding;
	char *file_path;
	size_t file_size;
};


enum ubwcp_accecss_dir {
	READ_FROM_UBWCP = 0,
	WRITE_TO_UBWCP
};

// both are a write function
typedef int (*write_fn) (char *image_out, char *image_in, size_t lin_size, Image_Attrs *image_attrs);

enum access_func_type {
	/*
	 * Will traverse in a linear direction from the start of image.
	 * Will access byte by byte.
	 * Will access the complete Y and UV data planes of the image, but it won't access
	 * any of the padding.
	*/
	BYTE_LINEAR_PLANE_FUNC_TYPE= 0,

	/*
	 * Will traverse in a linear direction from the start of image.
	 * Will access byte by byte.
	 * Will access the whole image, including any padding.
	*/
	BYTE_LINEAR_WHOLE_FUNC_TYPE,

	/*
	 * Will traverse in a linear direction from the start of image.
	 * Will access pixel by pixel, for each pixel it will read the Y, U and V components
	 * of the pixel.
	 * Will access the complete Y and UV data planes of the image, but it won't access
	 * any of the padding.
	*/
	PIXEL_LINEAR_PLANE_FUNC_TYPE,

	/*
	 * Will traverse in a vertical direction from the start of image, will start with
	 * the pixels in column 1 (from top to bottom), then will access the pixels in
	 * column 2 (from top to bottom), ...
	 *
	 * Will access pixel by pixel, for each pixel it will read the Y, U and V components
	 * of the pixel.
	 * Will access the complete Y and UV data planes of the image, but it won't access
	 * any of the padding.
	*/
	PIXEL_VERTICAL_PLANE_FUNC_TYPE,

	/*
	 * Will traverse in a vertical direction from the start of image, will start with
	 * the bytes in column 1 (from top to bottom), then will access the bytes in
	 * column 2 (from top to bottom), ...
	 *
	 * Each column will be 4 Bytes in width.
	 * It won't access any of the padding.
	*/
	BYTE_VERTICAL_COL4B_PLANE_FUNC_TYPE

};

struct access_descriptor {
	char *name;
	enum access_func_type func_type;
	write_fn write_func;
};

typedef struct ubwcp_test_meta {
	char *name;
	test_fn func; // Test function which is run for test
	int image_format; // Can be a UBWC-P Lib image format or ALL_UBWCP_FORMATS
	int image_id; // Can be a image id or ALL_IDS
	bool wait_for_input; // Only supported for certain single thread tests, default false
	bool perf_test; // Only suported in complete_rw_test and complete_ro_test, default false
	bool alloc_each_iter; // Whether to allocate the buffer for each test
			      // iteration, default false
	struct access_descriptor *access_desc; // Determines how the tests accesses the data in
					       // the image, defaults to BYTE_LINEAR_PLANE_FUNC_TYPE
	unsigned long priv1; // Used for test specific information
};

typedef struct ubwcp_perf {
	uint64_t alloc_avg_ns; // Average UBWC-P buffer allocation
	uint64_t free_avg_ns; // Average UBWC-P buffer free
	uint64_t set_buf_attrs_avg_ns; // Average set buf attrs
	uint64_t begin_read_cpu_avg_ns; // Begin CPU access call (read) average
	uint64_t end_read_cpu_avg_ns; // End CPU access call (read) average
	uint64_t begin_write_cpu_avg_ns; // Begin CPU access call (write) average
	uint64_t end_write_cpu_avg_ns; // End CPU access call (write) average
	uint64_t begin_read_cpu_num; // Number of begin (read) CPU access calls
	uint64_t begin_write_cpu_num; // Number of begin (write) CPU access calls
	uint64_t end_read_cpu_num; // Number of end (read) CPU access calls
	uint64_t end_write_cpu_num; // Number of end (write) CPU access calls
	uint64_t read_st_ns; // Per thread read start time
	uint64_t read_end_ns; // Per thread read end time
	uint64_t read_ns; // Total per thread read time for a test plan run
	uint64_t read_tp_ns; // Is the time it took for all reads to complete for a test plan run,
			     // used to calcualte the read bandwidth for all threads
	uint64_t read_bytes; // Total per thread bytes read for a test plan run
	uint64_t write_st_ns; // Per thread write start time
	uint64_t write_end_ns; // Per thread write end time
	uint64_t write_ns; // Total per thread write time for a test plan run
	uint64_t write_tp_ns; // Is the time it took for all writes to complete for a test plan run
			      // used to calcualte the write bandwidth for all threads
	uint64_t write_bytes; // Total per thread bytes written for a test plan run
};

typedef struct ubwcp_test {
	ubwcp_test_meta * test_meta; // Test meta associated with the test
	Image_Attrs * image_comp_attrs; // Attributes of the compressed image
	int image_fd; // dma-buf fd
	int image_format; // Actual image format to use for test
	int image_id; // Actual image id to use for test
	int num_iters; // Number of iterations of the test within the test plan
	int thread_num; // Unique thread number within the test plan
	int err_code;   // Error code returned by test
	ubwcp_perf perf; // Stores the results in a perf test
	void * data; // Test specific data
	sem_t * write_st_sem; // Used to synchronize threads before starting writes
	atomic_int * write_st_wait_cnt; // Number of threads to wait for
	sem_t * write_end_sem; // Used to synchronize threads after writes finished
	atomic_int * write_end_wait_cnt; // Number of threads to wait for
	sem_t * read_st_sem; // Used to synchronize threads before starting reads
	atomic_int * read_st_wait_cnt; // Number of threads to wait for
	sem_t * read_end_sem; // Used to synchronize threads after reads finished
	atomic_int * read_end_wait_cnt; // Number of threads to wait for
};

#define MAX_TEST_PLAN_TESTS	300

/*
 * A Test Plan contains 1 or more tests to run concurently
 * Test management and test running is done using
 * Test Plans.
 */
typedef struct ubwcp_test_plan {
	char * name;
	int test_plan_id; // Unique per test
	ubwcp_test_meta test_list[MAX_TEST_PLAN_TESTS];
	int num_tests;
	int num_test_iters;
	int num_test_plan_iters; // Setting to 0 is same 1
	bool share_image;
	bool pre_init_image; // Only currently supported with shared image
	sem_t write_st_sem;
	sem_t read_st_sem;
	sem_t write_end_sem;
	sem_t read_end_sem;
};

/////////////////////////////////////////////////////////
/* Structures used to manage the list of test images */

typedef struct UBWCP_image_format_list {
	UBWCPLib_Image_Format format;
	int num_images;
	Image_Attrs * image_list;
};

typedef struct UBWCP_image_list {
	UBWCP_image_format_list format_list[UBWCPLib_NUM_FORMATS];
};
/////////////////////////////////////////////////////////
// Globals

// The list of all the test images
UBWCP_image_list test_image_list;

BufferAllocator * allocator;

/////////////////////////////////////////////////////////
// Utils functions to help with test writting

static int get_image_attrs(bool linear, UBWCPLib_Image_Format format, int id, Image_Attrs **image_attrs)
{
	int i;
	int cnt = 0;
	int ret = -EINVAL;
	for (i = 0; i < test_image_list.format_list[format].num_images; i++) {
		if (test_image_list.format_list[format].image_list[i].linear == linear &&
		    test_image_list.format_list[format].image_list[i].id == id) {
			*image_attrs = &(test_image_list.format_list[format].image_list[i]);
			if (*image_attrs == NULL) {
				ret = -EINVAL;
				UBWCP_LOG(UBWCP_ERR, "get_image_attrs is NULL linear:%d, format:%d, id:%d", linear, (int)format, id);
			}
			ret = 0;
			break;
		}
	}

	if (ret != 0) {
		UBWCP_LOG(UBWCP_ERR, "get_image_attrs fail linear:%d, format:%d, id:%d", linear, (int)format, id);
	}

	return ret;
}

static void dump_image_diff(char *gen_image, char *golden_image, size_t size)
{
	int diff_amount = 0;
	int tmp_size = 65;
	int cp_size = 0;
	int i;
	int non_contig = 0;
	int last_idx = -1;

	for (int i=0; i < size; i++) {
		if (gen_image[i] != golden_image[i]) {
			if (last_idx != -1 &&
			    last_idx + 1 != i) {
				non_contig++;
			}
			last_idx = i;
			diff_amount++;
		}
	}
	UBWCP_LOG(UBWCP_WARN, "image diff %d bytes of size %d bytes, non_contig %d", diff_amount, size, non_contig);
}

char *FAIL_STR = "_fail";

char * get_fail_path(char *src_str)
{
	char *dest_str;
	size_t size = strlen(src_str) + strlen(FAIL_STR) + 1;

	dest_str = (char *)malloc(size);
	if (!dest_str)
		return NULL;

	strlcpy(dest_str, src_str, size);
	strlcat(dest_str, FAIL_STR, size);
	return dest_str;
}

static void dump_to_file(char *image_ptr, size_t image_size, char *file_path)
{
	FILE *filep;
	char * file_fail_path = get_fail_path(file_path);
	if (!file_fail_path)
		return;

	filep = fopen(file_fail_path, "w");
	if (filep == NULL) {
		UBWCP_LOG(UBWCP_ERR, "Failed to open path:%s", file_fail_path);
		return;
	}

	fwrite((const void*)image_ptr, 1, image_size, filep);
	UBWCP_LOG(UBWCP_WARN, "Dumped file:%s", file_fail_path);
	fclose(filep);
	free(file_fail_path);
}

// page 169 - Pixel Data Alignment
static int get_stride_bytes(UBWCPLib_Image_Format image_format, int stride, int *stride_bytes)

{
	int bytes = 0;

	switch (image_format) {
	case UBWCPLib_RGBA8888:
		bytes = 4 * stride;
		break;
	case UBWCPLib_LINEAR:
	case UBWCPLib_NV12:
	case UBWCPLib_NV12_Y:
	case UBWCPLib_NV12_UV:
	case UBWCPLib_NV124R:
	case UBWCPLib_NV124R_Y:
	case UBWCPLib_NV124R_UV:
		bytes = stride;
		break;
	case UBWCPLib_P010:
	case UBWCPLib_P010_Y:
	case UBWCPLib_P010_UV:
	case UBWCPLib_P016:
	case UBWCPLib_P016_Y:
	case UBWCPLib_P016_UV:
		bytes = 2 * stride;
		break;
	case UBWCPLib_TP10:
	case UBWCPLib_TP10_Y:
	case UBWCPLib_TP10_UV:
		// Use P010 stride for TP10 since P010 is used as format
		// for linear aperature
		bytes = 2 * stride;
		break;
	default:
		goto err;
	}

	*stride_bytes = bytes;
	return 0;

err:
	return -EINVAL;
}

static void init_buf_attrs(UBWCPLib_buf_attrs *buf_attrs, UBWCPLib_Image_Format image_format, unsigned int w,
		unsigned int h, unsigned int stride, unsigned int scanlines, size_t planar_padding)
{
	buf_attrs->image_format = image_format;
	buf_attrs->width = w;
	buf_attrs->height = h;
	buf_attrs->stride = stride;
	buf_attrs->scanlines = scanlines;
	buf_attrs->planar_padding = planar_padding;
}

static void has_planes(bool *has_y_plane, bool *has_uv_plane, Image_Attrs *image_attrs)
{
	if (image_attrs->format == UBWCPLib_RGBA8888) {
		*has_y_plane = true;
		*has_uv_plane = false;
	} else {
		*has_y_plane = true;
		*has_uv_plane = true;
	}
}

static int get_lin_y_pixel_bytes(UBWCPLib_Image_Format image_format, int *pixel_bytes)

{
	switch (image_format) {
	case UBWCPLib_RGBA8888:
		*pixel_bytes = 4;
		break;
	case UBWCPLib_LINEAR:
	case UBWCPLib_NV12:
	case UBWCPLib_NV12_Y:
	case UBWCPLib_NV124R:
	case UBWCPLib_NV124R_Y:
		*pixel_bytes = 1;
		break;
	case UBWCPLib_P010:
	case UBWCPLib_P010_Y:
	case UBWCPLib_TP10:
	case UBWCPLib_TP10_Y:
		*pixel_bytes = 2;
		break;
	default:
		goto err;
	}

	return 0;

err:
	UBWCP_LOG(UBWCP_ERR, "get_lin_y_pixel_bytes format:%d not supported\n", image_format);
	return -EINVAL;
}

static int get_lin_uv_pixel_bytes(UBWCPLib_Image_Format image_format, int *pixel_bytes)

{
	switch (image_format) {
	case UBWCPLib_LINEAR:
	case UBWCPLib_NV12:
	case UBWCPLib_NV12_UV:
	case UBWCPLib_NV124R:
	case UBWCPLib_NV124R_UV:
		*pixel_bytes = 2;
		break;
	case UBWCPLib_P010:
	case UBWCPLib_P010_UV:
	case UBWCPLib_TP10:
	case UBWCPLib_TP10_UV:
		*pixel_bytes = 4;
		break;
	default:
		goto err;
	}

	return 0;

err:
	UBWCP_LOG(UBWCP_ERR, "get_lin_uv_pixel_bytes format:%d not supported\n", image_format);
	return -EINVAL;
}


/*
 * Only compare the memory in the planes.
 * Don't compare memory in the stride, scanlines or planar padding regions
 */
int memcmp_planes(char *image_lin_raw, char *image_lin_ptr, Image_Attrs *image_attrs, int lin_size)
{
	bool has_y_plane = false;
	bool has_uv_plane = false;
	int offset, i;
	int cmp = 0;
	int y_plane_offset_const, y_plane_size, uv_plane_offset_const;

	int pixel_y_size = 0;
	size_t w_bytes;

	has_planes(&has_y_plane, &has_uv_plane, image_attrs);

	if (get_lin_y_pixel_bytes(image_attrs->format, &pixel_y_size))
		return -1;
	w_bytes = pixel_y_size * image_attrs->w;

	y_plane_offset_const = 0;
	y_plane_size = image_attrs->stride * image_attrs->scanlines;

	uv_plane_offset_const = y_plane_offset_const + y_plane_size + image_attrs->planar_padding;

	if (has_y_plane) {
		int y_cmp = 0;

		for (i = 0; i < image_attrs->h; i++) {
			offset = y_plane_offset_const + i * image_attrs->stride;
			y_cmp = memcmp(image_lin_raw + offset, image_lin_ptr + offset, w_bytes);
			if (cmp == 0 && y_cmp != 0) {
				cmp = y_cmp;
				break;
			}
		}
	}

	if (has_uv_plane) {
		int uv_cmp = 0;

		for (i = 0; i < image_attrs->h / 2; i++) {
			offset = uv_plane_offset_const + i * image_attrs->stride;
			uv_cmp = memcmp(image_lin_raw + offset, image_lin_ptr + offset, w_bytes);
			if (cmp == 0 && uv_cmp != 0) {
				cmp = uv_cmp;
				break;
			}
		}
	}

	return cmp;
}

uint64_t get_cur_time()
{
	struct timespec time;
	clock_gettime(CLOCK_BOOTTIME, &time);
	return 1000000000L * time.tv_sec + time.tv_nsec;
}

static void free_image(struct ubwcp_test *test)
{
	uint64_t start_ns, end_ns;

	if (test->test_meta->perf_test)
		start_ns = get_cur_time();

	close(test->image_fd);

	if (test->test_meta->perf_test) {
		end_ns = get_cur_time();
		test->perf.free_avg_ns += end_ns - start_ns;
	}
}

static int get_ubwcp_image(Image_Attrs *attrs, int format, int id, int *dma_buf_fd,
			   struct ubwcp_test *test, bool perf)
{
	int fd;
	int ret = 0;
	UBWCPLib_buf_attrs attrsUBWCP;
	void * session;
	uint64_t start_ns, end_ns;
	memset(&attrsUBWCP, 0, sizeof(UBWCPLib_buf_attrs));

	auto heap_list = allocator->GetDmabufHeapList();
	if (heap_list.find(DmabufUBWCPHeapName) == heap_list.end()) {
		UBWCP_LOG(UBWCP_ERR, "UBWC-P heap not supported\n");
	}

	if (perf)
		start_ns = get_cur_time();

	fd = allocator->Alloc(DmabufUBWCPHeapName, attrs->file_size);

	if (perf) {
		end_ns = get_cur_time();
		test->perf.alloc_avg_ns += end_ns - start_ns;
	}

	if (fd < 0) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_ERR, "allocator->Alloc failed, size:%d", attrs->file_size);
		goto err;
	}
	session = UBWCPLib_create_session();
	if (!session) {
		ret = -EINVAL;
		goto close_fd;
	}

	if ((UBWCPLib_Image_Format)format != UBWCPLib_LINEAR) {
		ret = UBWCPLib_validate_stride(session, attrs->stride, (UBWCPLib_Image_Format)format, attrs->w);
		if (ret) {
			UBWCP_LOG(UBWCP_ERR, "Stride invalide stride(bytes):%d, format:%d, width:%d", attrs->stride, format, attrs->w );
			goto close_session;
		}
	}

	init_buf_attrs(&attrsUBWCP, (UBWCPLib_Image_Format)format, attrs->w, attrs->h,  attrs->stride, attrs->scanlines, attrs->planar_padding);

	ret = UBWCPLib_set_buf_attrs(session, fd, &attrsUBWCP);
	if (ret) {
		UBWCP_LOG(UBWCP_ERR, "UBWCPLib_set_buf_attrs failed, ret:%d", ret);
		goto close_session;
	}

	UBWCPLib_destroy_session(session);
	*dma_buf_fd = fd;
	return 0;

close_session:
	UBWCPLib_destroy_session(session);

close_fd:
	close(fd);

err:
	return ret;
}

static int get_num_images(int image_format)
{
	return test_image_list.format_list[image_format].num_images;
}

static int init_image(bool linear, int format, int id, char **image, int *size)
{
	FILE *filep;
	char ch;
	char * data;
	size_t rbytes;

	Image_Attrs * image_attrs;

	int ret = get_image_attrs(linear, (UBWCPLib_Image_Format)format, id, &image_attrs);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_ERR, "get_image_attrs failed linear:%d format:%d id:%d", linear, format, id);
		goto err;
	}

	data = (char *) malloc((size_t)image_attrs->file_size);
	if (!data) {
		ret = -ENOMEM;
		goto err;
	}

	filep = fopen(image_attrs->file_path, "rb");
	if (filep == NULL) {
		UBWCP_LOG(UBWCP_ERR, "Failed to open path:%s", image_attrs->file_path);
		ret = -EINVAL;
		goto free_data;
	}

	rbytes = fread(data, 1, image_attrs->file_size, filep);
	fclose(filep);

	if (rbytes != image_attrs->file_size) {
		UBWCP_LOG(UBWCP_ERR, "File read fail read:%d size to read:%d file:%s", rbytes, image_attrs->file_size, image_attrs->file_path);
		ret = -EINVAL;
		goto free_data;
	}

	*image = data;
	*size = image_attrs->file_size;


	return 0;

free_data:
	free(data);
err:
	return ret;
}

static int pre_init_image(int image_fd, int image_format, int image_id)
{
	char * image_ubwc_raw = NULL;
	int ubwc_size;
	int ret = -EINVAL;
	void * session;
	char * image_ubwc_ptr;
	UBWCPLib_buf_attrs lib_image_attrs;

	ret = init_image(false, image_format, image_id, &image_ubwc_raw, &ubwc_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed ubwc format:%d id:%d", image_format,image_id);
		goto err;
	}

	Image_Attrs *image_attrs;
	ret = get_image_attrs(true, (UBWCPLib_Image_Format)image_format, image_id, &image_attrs);
	if (ret != 0)
		goto free_ubwc_raw;

	init_buf_attrs(&lib_image_attrs, UBWCPLib_LINEAR, image_attrs->w, image_attrs->h,  image_attrs->stride,
		image_attrs->scanlines, image_attrs->planar_padding);

	session = UBWCPLib_create_session();
	if (!session)
		goto free_ubwc_raw;

	ret = UBWCPLib_set_buf_attrs(session, image_fd, &lib_image_attrs);
	if (ret != 0)
		goto destroy_session;

	// Map the actual buffer
	image_ubwc_ptr =(char *) mmap(NULL, ubwc_size, PROT_READ | PROT_WRITE, MAP_SHARED, image_fd, 0);
	if (image_ubwc_ptr == MAP_FAILED) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_WARN, "Failed to mmap ubwc buffer");
		goto destroy_session;
	}

	//First write the compressed contents into the image buffer
	ret = allocator->CpuSyncStart(image_fd, kSyncReadWrite, NULL, NULL);
	if (ret != 0) {
		munmap(image_ubwc_ptr, ubwc_size);
		goto destroy_session;
	}

	memcpy(image_ubwc_ptr, image_ubwc_raw, ubwc_size);

	ret = allocator->CpuSyncEnd(image_fd, kSyncReadWrite, NULL, NULL);
        if (ret != 0) {
                munmap(image_ubwc_ptr, ubwc_size);
                goto destroy_session;
        }

	munmap(image_ubwc_ptr, ubwc_size);

	// Now set back to actual image format
	init_buf_attrs(&lib_image_attrs, (UBWCPLib_Image_Format)image_format, image_attrs->w, image_attrs->h,  image_attrs->stride,
		image_attrs->scanlines, image_attrs->planar_padding);

	ret = UBWCPLib_set_buf_attrs(session, image_fd, &lib_image_attrs);
	if (ret != 0)
		goto destroy_session;

destroy_session:
	UBWCPLib_destroy_session(session);

free_ubwc_raw:
	free(image_ubwc_raw);

err:
	return ret;
}

int set_buf_attrs(void *session, unsigned int dmabuf_fd, UBWCPLib_buf_attrs *attrs,
		  struct ubwcp_test *test, bool perf)
{
	int ret;
	uint64_t start_ns, end_ns;

	if (perf)
		start_ns = get_cur_time();

	ret = UBWCPLib_set_buf_attrs(session, dmabuf_fd, attrs);

	if (perf) {
		end_ns = get_cur_time();
		test->perf.set_buf_attrs_avg_ns += end_ns - start_ns;
	}

	return ret;
}

int cpu_sync_start(unsigned int dmabuf_fd, SyncType sync_type,
		   struct ubwcp_test *test, bool perf)
{
	int ret;
	uint64_t start_ns, end_ns;

	if (perf)
		start_ns = get_cur_time();

	ret = allocator->CpuSyncStart(dmabuf_fd, sync_type, NULL, NULL);

	if (perf) {
		end_ns = get_cur_time();

		if (sync_type == kSyncRead ||
		    sync_type == kSyncReadWrite) {
			test->perf.begin_read_cpu_avg_ns += end_ns - start_ns;
			test->perf.begin_read_cpu_num++;
		}

		if (sync_type == kSyncWrite ||
		    sync_type == kSyncReadWrite) {
			test->perf.begin_write_cpu_avg_ns += end_ns - start_ns;
			test->perf.begin_write_cpu_num++;
		}
	}
	return ret;
}

int cpu_sync_end(unsigned int dmabuf_fd, SyncType sync_type,
		 struct ubwcp_test *test, bool perf)
{
	int ret;
	uint64_t start_ns, end_ns;

	if (perf)
		start_ns = get_cur_time();

	ret = allocator->CpuSyncEnd(dmabuf_fd, sync_type, NULL, NULL);

	if (perf) {
		end_ns = get_cur_time();

		if (sync_type == kSyncRead ||
		    sync_type == kSyncReadWrite) {
			test->perf.end_read_cpu_avg_ns += end_ns - start_ns;
			test->perf.end_read_cpu_num++;
		}

		if (sync_type == kSyncWrite ||
		    sync_type == kSyncReadWrite) {
			test->perf.end_write_cpu_avg_ns += end_ns - start_ns;
			test->perf.end_write_cpu_num++;
		}
	}

	return ret;
}

/* Implementation of BYTE_LINEAR_PLANE_FUNC_TYPE access */
static int write_byte_linear_plane_acc_func(char *image_out, char *image_in, size_t lin_size,
	Image_Attrs *image_attrs)
{
	bool has_y_plane = false;
	bool has_uv_plane = false;
	int offset, i;
	int cmp = 0;
	int y_plane_offset_const, y_plane_size, uv_plane_offset_const;
	int pixel_y_size = 0;
	size_t w_bytes;

	has_planes(&has_y_plane, &has_uv_plane, image_attrs);

	if (get_lin_y_pixel_bytes(image_attrs->format, &pixel_y_size))
		return -1;
	w_bytes = pixel_y_size * image_attrs->w;

	y_plane_offset_const = 0;
	y_plane_size = image_attrs->stride * image_attrs->scanlines;

	uv_plane_offset_const = y_plane_offset_const + y_plane_size + image_attrs->planar_padding;

	if (has_y_plane) {
		for (i = 0; i < image_attrs->h; i++) {
			offset = y_plane_offset_const + i * image_attrs->stride;
			if (offset + w_bytes > lin_size) {
				UBWCP_LOG(UBWCP_ERR, "offset too large offset:%d len:%d\n",
					  offset, lin_size);
				return -EINVAL;
			}
			memcpy(image_out + offset, image_in + offset, w_bytes);
		}
	}

	if (has_uv_plane) {
		for (i = 0; i < image_attrs->h / 2; i++) {
			offset = uv_plane_offset_const + i * image_attrs->stride;
			if (offset + w_bytes > lin_size) {
				UBWCP_LOG(UBWCP_ERR, "offset too large offset:%d len:%d\n",
					  offset, lin_size);
				return -EINVAL;
			}
			memcpy(image_out + offset, image_in + offset, w_bytes);
		}
	}
	return 0;
}

struct access_descriptor BYTE_LINEAR_PLANE_ACCESS {
	.name = "Byte linear plane access",
	.func_type = BYTE_LINEAR_PLANE_FUNC_TYPE,
	.write_func = write_byte_linear_plane_acc_func
};

static int write_byte_vertical_common(char *image_out, char *image_in, size_t lin_size,
		        Image_Attrs *image_attrs, size_t column_size_b)

{
	bool has_y_plane = false;
	bool has_uv_plane = false;
	int offset, row, col, cp_size;
	int cmp = 0;
	int y_plane_offset_const, y_plane_size, uv_plane_offset_const;
	int pixel_y_size = 0;
	size_t w_bytes;

	has_planes(&has_y_plane, &has_uv_plane, image_attrs);

	if (get_lin_y_pixel_bytes(image_attrs->format, &pixel_y_size))
		return -1;
	w_bytes = pixel_y_size * image_attrs->w;

	y_plane_offset_const = 0;
	y_plane_size = image_attrs->stride * image_attrs->scanlines;

	uv_plane_offset_const = y_plane_offset_const + y_plane_size + image_attrs->planar_padding;

	if (has_y_plane) {
		for (col = 0; col < w_bytes; col += column_size_b) {
			cp_size = column_size_b;
			if (col + cp_size > w_bytes)
				cp_size = w_bytes - col;
			for (row = 0; row < image_attrs->h; row++) {
				offset = y_plane_offset_const + row * image_attrs->stride + col;
				memcpy(image_out + offset, image_in + offset, cp_size);
			}
		}
	}

	if (has_uv_plane) {
		for (col = 0; col < w_bytes; col += column_size_b) {
			cp_size = column_size_b;
			if (col + cp_size > w_bytes)
				cp_size = w_bytes - col;
			for (row = 0; row < image_attrs->h / 2; row++) {
				offset = uv_plane_offset_const + row * image_attrs->stride + col;
				memcpy(image_out + offset, image_in + offset, cp_size);
			}
		}
	}
	return 0;
}

static int write_byte_vertical_col4b_plane_acc_func(char *image_out, char *image_in, size_t lin_size,
	Image_Attrs *image_attrs)
{
	return write_byte_vertical_common(image_out, image_in, lin_size, image_attrs, 4);
}

struct access_descriptor BYTE_VERTICAL_COL4B_PLANE_ACCESS {
	.name = "Byte vertical col 4 bytes plane access",
	.func_type = BYTE_VERTICAL_COL4B_PLANE_FUNC_TYPE,
	.write_func = write_byte_vertical_col4b_plane_acc_func
};

/* Implementation of BYTE_LINEAR_WHOLE_FUNC_TYPE access */
static int write_byte_linear_whole_acc_func(char *image_out, char *image_in, size_t lin_size,
	Image_Attrs *image_attrs)
{
	memcpy(image_out, image_in, lin_size);
	return 0;
}

struct access_descriptor BYTE_LINEAR_WHOLE_ACCESS {
	.name = "Byte linear whole access",
	.func_type = BYTE_LINEAR_WHOLE_FUNC_TYPE,
	.write_func = write_byte_linear_whole_acc_func
};

/*
 * Will read the Y of pixel at row/col from image_in and will write the Y to
 * the same row/col location in image_out.
 */
inline int write_pixel_y_only(int row, int col, int y_pix_size, char *image_out, char *image_in,
			      size_t lin_size, Image_Attrs *image_attrs)
{

	int y_offset = (row * image_attrs->stride) + col * y_pix_size;

	if (y_offset >= lin_size) {
		UBWCP_LOG(UBWCP_ERR, "write_pixel_y_only invalid offset\n");
		return -EINVAL;
	}

	memcpy(image_out + y_offset, image_in + y_offset, y_pix_size);
	return 0;
}

/*
 * Will read the Y, U and V of pixel at row/col from image_in and will write the Y, U, V to
 * the same row/col location in image_out.
 * Only supports images with a sub sampling 4:2:0 layout.
 */
inline int write_pixel_subsample420(int row, int col, int y_pix_size, int uv_pix_size,
				    char *image_out, char *image_in, size_t lin_size, Image_Attrs *image_attrs)
{
	// Offset to pixel in bytes
	int y_offset;
	int u_offset;
	int v_offset;

	int uv_offset; // Offset of the UV plane in bytes

	// Supports writting in sub sampling 4:2:0 layout

	// Variables in pixel units
	// Example is for an image of width 6
	// Block size is 4 pixels (2x2)
	int super_block_size; // Number of Y pixels in a UV row (example 24)
	int block_row_size;   // Number of Y pixels in a row of blocks (example 12)
	int super_block_offset; // Pixel offset in the Y super block
	int row_block_offset; // Pixel offset in the Y block row
	int uv_row_offset; // Pixel offset in the UV plane row
	int uv_row; // UV row number

	int uv_row_size_bytes; // UV row size in bytes

	// calculate y
	y_offset = (row * image_attrs->stride) + col * y_pix_size;

	// calcualte u and v
	uv_offset = image_attrs->stride * image_attrs->scanlines + image_attrs->planar_padding;

	super_block_size = image_attrs->w * 4;
	block_row_size = image_attrs->w * 2;

	super_block_offset = ((row * image_attrs->w) + col) % super_block_size;
	row_block_offset = super_block_offset % block_row_size;

	// We divide by 2 because each UV pixel is 2 Y pixels wide
	if (super_block_offset < block_row_size) {
		//First block row
		if (row_block_offset < image_attrs->w)
			// First row in block
			uv_row_offset = row_block_offset / 2;
		else
			// Second row in block
			uv_row_offset = (row_block_offset - image_attrs->w) / 2;
	} else {
		//Second block row
		if (row_block_offset < image_attrs->w)
			// First row in block
			uv_row_offset = (image_attrs->w / 2) + row_block_offset / 2;
		else
			// Second row in block
			uv_row_offset = (image_attrs->w / 2) + (row_block_offset - image_attrs->w) / 2;
	}

	// Each UV row represents 4 Y rows
	uv_row = (row / 4);

	// Multiply by 2 since a UV row is made up of a U and a Y row.
	uv_row_size_bytes = image_attrs->stride * 2;

	// We divide uv_pix_size / 2 since we are only dealing with u
	u_offset = uv_offset + (uv_row * uv_row_size_bytes) + (uv_row_offset * (uv_pix_size / 2));

	v_offset = u_offset + image_attrs->stride;

	if (y_offset >= lin_size || u_offset >= lin_size || v_offset >= lin_size) {
		UBWCP_LOG(UBWCP_ERR, "write_pixel_subsample420 invalid offset\n");
		return -EINVAL;
	}

	memcpy(image_out + y_offset, image_in + y_offset, y_pix_size);
	memcpy(image_out + u_offset, image_in + u_offset, uv_pix_size / 2);
	memcpy(image_out + v_offset, image_in + v_offset, uv_pix_size / 2);

	return 0;
}

/* Implementation of PIXEL_LINEAR_PLANE_FUNC_TYPE access */
static int write_pixel_linear_plane_acc_func(char *image_out, char *image_in, size_t lin_size,
	Image_Attrs *image_attrs)
{
	int ret = 0;
	int pixel_y_size = 0;
	int pixel_uv_size = 0;
	bool has_y_plane, has_uv_plane;

	has_planes(&has_y_plane, &has_uv_plane, image_attrs);
	ret = get_lin_y_pixel_bytes(image_attrs->format, &pixel_y_size);
	if (ret)
		goto err;

	if (has_uv_plane) {
		ret = get_lin_uv_pixel_bytes(image_attrs->format, &pixel_uv_size);
		if (ret)
			goto err;
	}

	for (int row = 0; row < image_attrs->h; row++) {
		for (int col = 0; col < image_attrs->w; col++) {
			if (!has_uv_plane)
				ret = write_pixel_y_only(row, col, pixel_y_size, image_out,
						         image_in, lin_size, image_attrs);
			else
				ret = write_pixel_subsample420(row, col, pixel_y_size, pixel_uv_size,
						    image_out, image_in, lin_size, image_attrs);

			if (ret != 0)
				goto err;
		}
	}

	return 0;
err:
	return ret;

}

struct access_descriptor PIXEL_LINEAR_PLANE_ACCESS {
	.name = "Pixel linear plane access",
	.func_type = PIXEL_LINEAR_PLANE_FUNC_TYPE,
	.write_func = write_pixel_linear_plane_acc_func
};

/* Implementation of PIXEL_VERTICAL_PLANE_FUNC_TYPE access */
static int write_pixel_vertical_plane_acc_func(char *image_out, char *image_in, size_t lin_size,
	Image_Attrs *image_attrs)
{
	int ret = 0;
	int pixel_y_size = 0;
	int pixel_uv_size = 0;
	bool has_y_plane, has_uv_plane;

	has_planes(&has_y_plane, &has_uv_plane, image_attrs);

	ret = get_lin_y_pixel_bytes(image_attrs->format, &pixel_y_size);
	if (ret)
		goto err;

	if (has_uv_plane) {
		ret = get_lin_uv_pixel_bytes(image_attrs->format, &pixel_uv_size);
		if (ret)
			goto err;
	}

	for (int col = 0; col < image_attrs->w; col++) {
		for (int row = 0; row < image_attrs->h; row++) {
			if (!has_uv_plane)
				ret = write_pixel_y_only(row, col, pixel_y_size, image_out,
						image_in, lin_size, image_attrs);
			else
				ret = write_pixel_subsample420(row, col, pixel_y_size, pixel_uv_size,
						image_out, image_in, lin_size, image_attrs);

			if (ret != 0)
				goto err;
		}
	}

	return 0;

err:
	return ret;

}

struct access_descriptor PIXEL_VERTICAL_PLANE_ACCESS {
	.name = "Pixel vertical plane access",
	.func_type = PIXEL_VERTICAL_PLANE_FUNC_TYPE,
	.write_func = write_pixel_vertical_plane_acc_func
};

/*
 * Will write contents of image_in to *image_out using provided access descriptor.
 * Access direction READ_FROM_UBWCP is used when image_in is a UBWC-P buffer and *image_out
 * is a non UBWC-P buffer.
 * Access direction WRITE_TO_UBWCP is used when image_in is a non UBWC-P buffer and
 * image_out is a UBWC-P buffer.
 * If *image_out is NULL it will allocate a non UBWC-P buffer for it.
 */
static int image_write(char *image_out, char *image_in, size_t lin_size, struct access_descriptor *access_desc,
		      Image_Attrs *image_attrs, bool perf_test, ubwcp_perf *perf, enum ubwcp_accecss_dir access_dir)
{
	uint64_t start_ns, end_ns;
	int ret = 0;

	if (perf_test) {
		start_ns = get_cur_time();
	}

	if (access_desc)
		ret = access_desc->write_func(image_out, image_in, lin_size, image_attrs);
	else
		/* Default to byte linear plane for backward compatibility */
		ret = write_byte_linear_plane_acc_func(image_out, image_in, lin_size, image_attrs);

	if (perf_test && ret == 0) {
		end_ns = get_cur_time();
		if (access_dir == READ_FROM_UBWCP) {
			perf->read_ns += end_ns - start_ns;
			perf->read_st_ns =  start_ns;
			perf->read_end_ns = end_ns;
			perf->read_bytes += lin_size;
		} else {
			perf->write_ns += end_ns - start_ns;
			perf->write_st_ns =  start_ns;
			perf->write_end_ns = end_ns;
			perf->write_bytes += lin_size;
		}
	}

	return ret;
}

static int validate_image(char *image_out, char *image_golden, Image_Attrs *image_attrs, int lin_size,
		bool dump_diff_to_file, bool pause_on_failure, int image_format, int image_id)
{
	int ret = 0;
	int cmp = memcmp_planes(image_golden, image_out, image_attrs, lin_size);
	if (cmp) {
		if (dump_diff_to_file) {
			dump_image_diff(image_out, image_golden, lin_size);
			dump_to_file(image_out, lin_size, image_attrs->file_path);
		}
		UBWCP_LOG(UBWCP_WARN, "linear img:%d indx:%d cmp failed", image_format, image_id);
		if (pause_on_failure) {
			UBWCP_LOG(UBWCP_ERR, "Test failed & blocked, press enter to continue:");
			fflush(0);
			getchar();
		}
		ret = -EINVAL;
	}

	return ret;
}

// returns alignment in bytes
int get_expected_stride_alignment(UBWCPLib_Image_Format format, size_t *alignment)
{
	int ret = 0;

	switch (format) {
	case UBWCPLib_LINEAR:
		*alignment = 1;
		break;
	case UBWCPLib_NV12:
	case UBWCPLib_NV12_Y:
	case UBWCPLib_NV12_UV:
		*alignment = 128;
		break;
	case UBWCPLib_NV124R:
	case UBWCPLib_NV124R_Y:
	case UBWCPLib_NV124R_UV:
	case UBWCPLib_RGBA8888:
	case UBWCPLib_P010:
	case UBWCPLib_P010_Y:
	case UBWCPLib_P010_UV:
		*alignment = 256;
		break;
	case UBWCPLib_TP10:
	case UBWCPLib_TP10_Y:
	case UBWCPLib_TP10_UV:
		*alignment = 64;
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

/*
 * Used to synchronize n threads
 * The threads entering this function will block until
 * 'wait_cnt' threads have entered this function, at
 * which point all the threads will be woken up.
 */
static int synchronize_test_threads(sem_t *sem, atomic_int *wait_cnt)
{
	int ret = 0;
	int prev_wait_cnt = atomic_fetch_sub(wait_cnt, 1);

	if (prev_wait_cnt > 1) {
		// Other threads haven't arrived yet so
		// wait for the other thread(s)
		ret = sem_wait(sem);
		if (ret != 0) {
			goto end;
		}
	}

	// Wake up the next waiting thread.
	// The next thread that wakes up will wake up the next
	// thread and so on which will unlock all threads waiting
	// on this semaphore
	ret = sem_post(sem);

end:
	return ret;
}

/*
 * This function should be called on any thread
 * who's test fails to ensure no threads are blocked on
 * this this thread.
 */
static void unblock_test_threads(struct ubwcp_test *test)
{
	if (test->write_st_wait_cnt)
		atomic_fetch_sub(test->write_st_wait_cnt, 1);

	if (test->write_end_wait_cnt)
		atomic_fetch_sub(test->write_end_wait_cnt, 1);

	if (test->read_st_wait_cnt)
		atomic_fetch_sub(test->read_st_wait_cnt, 1);

	if (test->read_end_wait_cnt)
		atomic_fetch_sub(test->read_end_wait_cnt, 1);

	if (test->write_st_sem)
		sem_post(test->write_st_sem);

	if (test->write_end_sem)
		sem_post(test->write_end_sem);

	if (test->read_st_sem)
		sem_post(test->read_st_sem);

	if (test->read_end_sem)
		sem_post(test->read_end_sem);
}

/////////////////////////////////////////////////////////
// Test implementation functions

/*
 * Will write the golden linear copy of the image to the UBWC-P buffer using the provided accessor function.
 * Will then use UBWC-P to read back the contents of the UBWC-P buffer using the provided accessor function
 * and will compare with the golden linear image.
 */
int complete_rw_test_common(struct ubwcp_test *test, int image_fd, bool skip_end_cpu_access)
{
	char * image_lin_raw = NULL;
	char * image_ubwc_raw = NULL;
	int lin_size, ubwc_size;
	int ret = -EINVAL;
	int cmp;
	void * session;
	char * image_ubwc_ptr;
	UBWCPLib_buf_attrs lib_image_attrs;
	char * image_lin_ptr;
	char * image_lin_out;

	ret = init_image(true, test->image_format, test->image_id, &image_lin_raw, &lin_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed linear format:%d id:%d", (int)test->image_format, test->image_id);
		goto err;
	}

	image_lin_out = (char *)malloc(lin_size);
	if (!image_lin_out)
		goto free_lin_raw;

	// Fault in pages so it doesn't impact performance measurement
	memset(image_lin_out, 'a', lin_size);

	ret = init_image(false, test->image_format, test->image_id, &image_ubwc_raw, &ubwc_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed ubwc format:%d id:%d", test->image_format, test->image_id);
		goto free_lin_out;
	}

	Image_Attrs *image_attrs;
	ret = get_image_attrs(true, (UBWCPLib_Image_Format)test->image_format, test->image_id, &image_attrs);
	if (ret != 0)
		goto free_ubwc_raw;

	init_buf_attrs(&lib_image_attrs, image_attrs->format, image_attrs->w, image_attrs->h,  image_attrs->stride, image_attrs->scanlines, image_attrs->planar_padding);

	session = UBWCPLib_create_session();
	if (!session)
		goto free_ubwc_raw;

	// Test UBWC-P read
	image_lin_ptr =(char *) mmap(NULL, lin_size, PROT_READ | PROT_WRITE, MAP_SHARED, image_fd, 0);
	if (image_lin_ptr == MAP_FAILED) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_WARN, "Failed to mmap linear buffer");
		goto destroy_session;
	}

	//First write the linear contents into the image buffer
	ret = cpu_sync_start(image_fd, kSyncWrite, test, test->test_meta->perf_test);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto destroy_session;
	}

	if (test->test_meta->wait_for_input) {
		UBWCP_LOG(UBWCP_ERR, "Test blocked, press enter to continue:");
		fflush(0);
		getchar();
	}

	if (test->write_st_sem) {
		// Synchronize the threads just before the write
		ret = synchronize_test_threads(test->write_st_sem, test->write_st_wait_cnt);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto destroy_session;
		}
	}

	// Read from non UBWC-P buffer and write contents into UBWC-P buffer
	ret = image_write(image_lin_ptr, image_lin_raw, lin_size, test->test_meta->access_desc, image_attrs,
			 test->test_meta->perf_test, &test->perf, WRITE_TO_UBWCP);

	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto destroy_session;
	}

	if (test->write_end_sem) {
		// Synchronize the threads after the write
		ret = synchronize_test_threads(test->write_end_sem, test->write_end_wait_cnt);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto destroy_session;
		}
	}

	//Ensure writes are compressed and written into buffer
	ret = cpu_sync_end(image_fd, kSyncWrite, test, test->test_meta->perf_test);
        if (ret != 0) {
                munmap(image_lin_ptr, lin_size);
                goto destroy_session;
        }

	// Test UBWC-P read
	// Test that UBWC-P can succesfully uncompress the data in the image buffer
	ret = cpu_sync_start(image_fd, kSyncRead, test, test->test_meta->perf_test);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto destroy_session;
	}

	if (test->read_st_sem) {
		// Synchronize the threads just before the read
		ret = synchronize_test_threads(test->read_st_sem, test->read_st_wait_cnt);
			if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto destroy_session;
		}
	}
	// Read from UBWC-P buffer and write contents into non UBWC-P buffer
	ret = image_write(image_lin_out, image_lin_ptr, lin_size, test->test_meta->access_desc, image_attrs,
			 test->test_meta->perf_test, &test->perf, READ_FROM_UBWCP);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto destroy_session;
	}

	if (test->read_end_sem) {
		// Synchronize the threads just after the read
		ret = synchronize_test_threads(test->read_end_sem, test->read_end_wait_cnt);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto destroy_session;
		}
	}

	ret = validate_image(image_lin_out, image_lin_raw, image_attrs, lin_size, dump_diff_to_file, pause_on_failure,
			     test->image_format, test->image_id);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto destroy_session;
	}

	if (!skip_end_cpu_access) {
		ret = cpu_sync_end(image_fd, kSyncRead, test, test->test_meta->perf_test);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto destroy_session;
		}
	}

	munmap(image_lin_ptr, lin_size);

	// Test UBWC-P write
	// Enusre the compressed data is properly written
	// Disable for now until we are sure this type of comparison is
	// supported.
	if (false) {
	init_buf_attrs(&lib_image_attrs, UBWCPLib_LINEAR, image_attrs->w, image_attrs->h,  image_attrs->stride, image_attrs->scanlines, image_attrs->planar_padding);

	ret = set_buf_attrs(session, image_fd, &lib_image_attrs, test, test->test_meta->perf_test);
	if (ret != 0)
		goto destroy_session;

	image_ubwc_ptr =(char *) mmap(NULL, ubwc_size, PROT_READ | PROT_WRITE, MAP_SHARED, image_fd, 0 );
        if (image_ubwc_ptr == MAP_FAILED) {
                ret = -EINVAL;
                UBWCP_LOG(UBWCP_WARN, "Failed to mmap ubwc buffer");
                goto destroy_session;
        }

	ret = cpu_sync_start(image_fd, kSyncReadWrite, test, test->test_meta->perf_test);
	if (ret != 0) {
		munmap(image_ubwc_ptr, ubwc_size);
		goto destroy_session;
	}

	cmp = memcmp(image_ubwc_raw, image_ubwc_ptr, ubwc_size);
	if (cmp) {
		if (dump_diff_to_file) {
			Image_Attrs *image_ubwc_attrs;
			dump_image_diff(image_ubwc_ptr, image_ubwc_raw, ubwc_size);
			ret = get_image_attrs(false, (UBWCPLib_Image_Format)test->image_format, test->image_id, &image_ubwc_attrs);
			if (ret == 0) {
				dump_to_file(image_ubwc_ptr, ubwc_size, image_ubwc_attrs->file_path);
			}
		}
		munmap(image_ubwc_ptr, ubwc_size);
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_WARN, "ubwc img:%d indx:%d cmp failed", test->image_format, test->image_id);
		goto destroy_session;
	}

	ret = cpu_sync_end(image_fd, kSyncReadWrite, test, test->test_meta->perf_test);
	if (ret != 0) {
		munmap(image_ubwc_ptr, ubwc_size);
		goto destroy_session;
	}

	munmap(image_ubwc_ptr, ubwc_size);
	}
destroy_session:
	UBWCPLib_destroy_session(session);

free_ubwc_raw:
	free(image_ubwc_raw);

free_lin_out:
	free(image_lin_out);

free_lin_raw:
	free(image_lin_raw);

err:
	return ret;
}

/*
 * Will write the golden linear copy of the image to the UBWC-P buffer using a linear mapping.
 * Will then use UBWC-P to read back the contents of the UBWC-P buffer using the provided accessor function
 * and will compare with the golden linear image.
 */
int complete_ro_test_common(struct ubwcp_test *test, int image_fd, bool skip_end_cpu_access)
{
	char * image_lin_raw = NULL;
	char * image_ubwc_raw = NULL;
	char * image_lin_out = NULL;
	int lin_size, ubwc_size;
	int ret = -EINVAL;
	int cmp;
	void * session;
	char * image_ubwc_ptr;
	UBWCPLib_buf_attrs lib_image_attrs;
	char * image_lin_ptr;

	ret = init_image(true, test->image_format, test->image_id, &image_lin_raw, &lin_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed linear format:%d id:%d", (int)test->image_format, test->image_id);
		goto err;
	}

	image_lin_out = (char *)malloc(lin_size);
	if (!image_lin_out)
		goto free_lin_raw;

	// Fault in pages so it doesn't impact performance measurement
	memset(image_lin_out, 'a', lin_size);

	ret = init_image(false, test->image_format, test->image_id, &image_ubwc_raw, &ubwc_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed ubwc format:%d id:%d", test->image_format, test->image_id);
		goto free_lin_out;
	}

	Image_Attrs *image_attrs;
	ret = get_image_attrs(true, (UBWCPLib_Image_Format)test->image_format, test->image_id, &image_attrs);
	if (ret != 0)
		goto free_ubwc_raw;

	init_buf_attrs(&lib_image_attrs, UBWCPLib_LINEAR, image_attrs->w, image_attrs->h,  image_attrs->stride,
		image_attrs->scanlines, image_attrs->planar_padding);

	session = UBWCPLib_create_session();
	if (!session)
		goto free_ubwc_raw;

	ret = UBWCPLib_set_buf_attrs(session, image_fd, &lib_image_attrs);
	if (ret != 0)
		goto destroy_session;

	// Map the actual buffer
	image_ubwc_ptr =(char *) mmap(NULL, ubwc_size, PROT_READ | PROT_WRITE, MAP_SHARED, image_fd, 0);
	if (image_ubwc_ptr == MAP_FAILED) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_WARN, "Failed to mmap ubwc buffer");
		goto destroy_session;
	}

	//First write the compressed contents into the image buffer
	ret = allocator->CpuSyncStart(image_fd, kSyncReadWrite, NULL, NULL);
	if (ret != 0) {
		munmap(image_ubwc_ptr, ubwc_size);
		goto destroy_session;
	}

	memcpy(image_ubwc_ptr, image_ubwc_raw, ubwc_size);

	ret = allocator->CpuSyncEnd(image_fd, kSyncReadWrite, NULL, NULL);
        if (ret != 0) {
                munmap(image_ubwc_ptr, ubwc_size);
                goto destroy_session;
        }

	munmap(image_ubwc_ptr, ubwc_size);

	// Test UBWC-P read
	// Access the buffer using UBWC-P
	init_buf_attrs(&lib_image_attrs, image_attrs->format, image_attrs->w, image_attrs->h,  image_attrs->stride, image_attrs->scanlines, image_attrs->planar_padding);

	ret = set_buf_attrs(session, image_fd, &lib_image_attrs, test, test->test_meta->perf_test);
	if (ret != 0)
		goto destroy_session;

	// Test UBWC-P read
	// Test that UBWC-P can succesfully uncompress the data in the image buffer
	image_lin_ptr =(char *) mmap(NULL, lin_size, PROT_READ | PROT_WRITE, MAP_SHARED, image_fd, 0);
	if (image_lin_ptr == MAP_FAILED) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_WARN, "Failed to mmap linear buffer");
		goto destroy_session;
	}

	ret = cpu_sync_start(image_fd, kSyncRead, test, test->test_meta->perf_test);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto destroy_session;
	}

	if (test->test_meta->wait_for_input) {
		UBWCP_LOG(UBWCP_ERR, "Test blocked, press enter to continue:");
		fflush(0);
		getchar();
	}

	if (test->read_st_sem) {
		// Synchronize the threads just before the read
		ret = synchronize_test_threads(test->read_st_sem, test->read_st_wait_cnt);
			if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto destroy_session;
		}
	}

	// Read from UBWC-P buffer and write contents into non UBWC-P buffer
	ret = image_write(image_lin_out, image_lin_ptr, lin_size, test->test_meta->access_desc, image_attrs,
			 test->test_meta->perf_test, &test->perf, READ_FROM_UBWCP);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto destroy_session;
	}

	if (test->read_end_sem) {
		// Synchronize the threads just after the read
		ret = synchronize_test_threads(test->read_end_sem, test->read_end_wait_cnt);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto destroy_session;
		}
	}

	ret = validate_image(image_lin_out, image_lin_raw, image_attrs, lin_size, dump_diff_to_file, pause_on_failure,
			     test->image_format, test->image_id);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto destroy_session;
	}

	if (!skip_end_cpu_access) {
		ret = cpu_sync_end(image_fd, kSyncRead, test, test->test_meta->perf_test);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto destroy_session;
		}
	}

	munmap(image_lin_ptr, lin_size);

destroy_session:
	UBWCPLib_destroy_session(session);

free_ubwc_raw:
	free(image_ubwc_raw);

free_lin_out:
	free(image_lin_out);

free_lin_raw:
	free(image_lin_raw);

err:
	return ret;
}

int complete_rw_test(struct ubwcp_test *test)
{
	return complete_rw_test_common(test, test->image_fd, false);
}

int complete_ro_test(struct ubwcp_test *test)
{
	return complete_ro_test_common(test, test->image_fd, false);
}

/*
 * Will sequentially run either the read-only or the write test on
 * the provided images.
 * This test will ensure that each image has its own unique ULA PA range
 * by only freeing the images at the end of the test.
 * This tests supports keeping up to 256 descriptors programmed at the
 * same time.
 */
int ulapa_range_test_common(struct ubwcp_test *test, bool ro_test)
{
	Image_Attrs *image_lin_attrs;
	Image_Attrs *image_comp_attrs;
	bool skip_sync_end = true; // Keep the range translation programmed
	int *image_fd_list;
	int i;
	int ret = get_image_attrs(true, (UBWCPLib_Image_Format)test->image_format,  test->image_id, &image_lin_attrs);

	if (ret != 0) {
		UBWCP_LOG(UBWCP_ERR, "get_image_attrs failed linear format:%d id:%d", test->image_format, test->image_id);
		return -EINVAL;
	}

	ret = get_image_attrs(false, (UBWCPLib_Image_Format)test->image_format,  test->image_id, &image_comp_attrs);

	if (ret != 0) {
		UBWCP_LOG(UBWCP_ERR, "get_image_attrs failed comp format:%d id:%d", test->image_format, test->image_id);
		return -EINVAL;
	}
	unsigned long num_images = test->test_meta->priv1 / image_lin_attrs->file_size;

	if (num_images < 1) {
		UBWCP_LOG(UBWCP_ERR, "Error: num calcuated images < 1");
		return -EINVAL;
	}
	if (num_images > 256) {
		// Can't exceed number of concurent range translations
		UBWCP_LOG(UBWCP_INFO, "Don't keep all range translations programed, num_images:%d", num_images);
		skip_sync_end = false;
	}

	image_fd_list = (int *) malloc(sizeof(int) * num_images);
	if (!image_fd_list) {
		UBWCP_LOG(UBWCP_ERR, "Error: failed to allocate array size:", sizeof(int) * num_images);
		return -ENOMEM;
	}

	image_fd_list[0] = test->image_fd;

	for (i = 0; i < num_images; i++) {
		if (i > 0) {
			int image_fd;
			ret = get_ubwcp_image(image_comp_attrs, test->image_format, test->image_id, &image_fd, test, test->test_meta->perf_test); 
			if (ret != 0) {
				UBWCP_LOG(UBWCP_ERR, "Failed to get UBWC-P image, i:%d", i);
				goto free_images;
			}
			image_fd_list[i] = image_fd;
		}
		if (ro_test)
			ret = complete_ro_test_common(test, image_fd_list[i], skip_sync_end);
		else
			ret = complete_rw_test_common(test, image_fd_list[i], skip_sync_end);

		if (ret != 0) {
			UBWCP_LOG(UBWCP_ERR, "complete_ro_test_common failed image i:%d, ro_test:%d", i, ro_test);
			goto free_images;
		} else {
			UBWCP_LOG(UBWCP_INFO, "complete_ro_test_common passed i:%d/%d, ro_test:%d", i, num_images - 1, ro_test);
		}
	}
	i--;

free_images:
	for (int j = 1; j <= i; j++) {
		close(image_fd_list[j]);
	}

	return ret;

}

int ulapa_range_ro_test(struct ubwcp_test *test)
{
	return ulapa_range_test_common(test, true);
}

int ulapa_range_rw_test(struct ubwcp_test *test)
{
	return ulapa_range_test_common(test, false);
}

// largest common tile height across RGBA888, P010, TP10 and NV12
#define COMMON_TILE_HEIGHT	8

/*
 * Tests UBWC-P reading from a shared image by multiple threads
 * with no tiles shared between threads.
 * This test shares an image with up to 4 threads.
 * It partitions the image into 4 sections and each thread only reads
 * from its section.
 * The section is initialized using a linear mapping.
 * The contents read from each section using UBWC-P are compared against the
 * equivalent section in the golden image.
 * Each section is tile aligned.
 * The data is accessed in a linear byte order.
 */
int share4_linear_ro_test(struct ubwcp_test *test)
{
	char * image_lin_raw = NULL;
	int lin_size;
	int ret = -EINVAL;
	int cmp;
	char * image_lin_ptr;
	int num_threads = 4; // number of threads supported by test
	int q_height, n_tile_h, thread_height, thread_read_size, thread_read_offset;
	int bytes_per_pixel;

	if (test->thread_num >= num_threads) {
		UBWCP_LOG(UBWCP_WARN, "Test only supports up to 4 threads, thread id:%d", test->thread_num);
		goto err;
	}

	if (test->test_meta->access_desc != NULL) {
		UBWCP_LOG(UBWCP_WARN, "This test doesn't support overriding the access type");
		goto err;
	}

	ret = get_lin_y_pixel_bytes((UBWCPLib_Image_Format)test->image_format, &bytes_per_pixel);
	if (ret)
		goto err;

	ret = init_image(true, test->image_format, test->image_id, &image_lin_raw, &lin_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed linear format:%d id:%d", (int)test->image_format, test->image_id);
		goto err;
	}

	Image_Attrs *image_attrs;
	ret = get_image_attrs(true, (UBWCPLib_Image_Format)test->image_format, test->image_id, &image_attrs);
	if (ret != 0)
		goto free_lin_raw;

	if (image_attrs->stride != (image_attrs->w * bytes_per_pixel)) {
		ret = 0;
		UBWCP_LOG(UBWCP_INFO, "share4_linear_ro_test test doesn't support stride padding format:%d id:%d",
				(int)test->image_format, test->image_id);
		goto free_lin_raw;
	}

	q_height = image_attrs->h / num_threads;
	n_tile_h = q_height / COMMON_TILE_HEIGHT;
	thread_height = n_tile_h * COMMON_TILE_HEIGHT;
	thread_read_size = thread_height * image_attrs->stride;

	thread_read_offset = test->thread_num * thread_read_size;

	// Test UBWC-P read
	// Test that UBWC-P can succesfully uncompress the data in the image buffer
	image_lin_ptr =(char *) mmap(NULL, lin_size, PROT_READ | PROT_WRITE, MAP_SHARED, test->image_fd, 0);
	if (image_lin_ptr == MAP_FAILED) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_WARN, "Failed to mmap linear buffer");
		goto free_lin_raw;
	}

	ret = allocator->CpuSyncStart(test->image_fd, kSyncReadWrite, NULL, NULL);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto free_lin_raw;
	}

	cmp = memcmp(image_lin_raw + thread_read_offset, image_lin_ptr + thread_read_offset, thread_read_size);
	if (cmp) {
		if (dump_diff_to_file) {
			dump_image_diff(image_lin_ptr + thread_read_offset, image_lin_raw + thread_read_offset, thread_read_size);
			dump_to_file(image_lin_ptr, lin_size, image_attrs->file_path);
		}
		munmap(image_lin_ptr, lin_size);
		UBWCP_LOG(UBWCP_WARN, "linear img:%d indx:%d read offset:%d read size:%d cmp failed", test->image_format,
			test->image_id, thread_read_offset, thread_read_size);
		if (pause_on_failure) {
			UBWCP_LOG(UBWCP_ERR, "Test failed & blocked, press enter to continue:");
			fflush(0);
			getchar();
		}
		ret = -EINVAL;
		goto free_lin_raw;
	}

	ret = allocator->CpuSyncEnd(test->image_fd, kSyncReadWrite, NULL, NULL);
        if (ret != 0) {
                munmap(image_lin_ptr, lin_size);
                goto free_lin_raw;
        }

	munmap(image_lin_ptr, lin_size);

free_lin_raw:
	free(image_lin_raw);

err:
	return ret;
}

int get_tile_w_bytes(int format, int *w_bytes)
{
	int ret = 0;

	switch(format) {
		case UBWCPLib_RGBA8888:
		case UBWCPLib_P010:
		case UBWCPLib_TP10:
		case UBWCPLib_NV124R:
			*w_bytes = 64;
			break;
		case UBWCPLib_NV12:
			*w_bytes = 32;
			break;
		default:
			ret = -EINVAL;
			UBWCP_LOG(UBWCP_WARN, "get_tile_w_bytes unsuported format:%d", format);
			break;
	}

	return ret;
}

int get_tile_h(int format, int *h)
{
	int ret = 0;

	switch(format) {
		case UBWCPLib_RGBA8888:
		case UBWCPLib_P010:
		case UBWCPLib_TP10:
		case UBWCPLib_NV124R:
			*h = 4;
			break;
		case UBWCPLib_NV12:
			*h = 8;
			break;
		default:
			ret = -EINVAL;
			UBWCP_LOG(UBWCP_WARN, "get_tile_h unsuported format:%d", format);
			break;
	}

	return ret;
}

/*
 * Tests UBWC-P reading from a shared tile in an image shared by multiple threads.
 * This test shares an image with up to 2 threads.
 * It partitions the first tile in the image into 2 sections and each thread only reads
 * from its section.
 * The section is intialized using a linear mapping.
 * The contents read from each section using UBWC-P are compared against the
 * equivalent section in the golden image.
 * The data is accessed in a linear byte order.
 */
int share2_linear_same_tile_ro_test(struct ubwcp_test *test)
{
	char * image_lin_raw = NULL;
	int lin_size;
	int ret = -EINVAL;
	int cmp;
	UBWCPLib_buf_attrs lib_image_attrs;
	char * image_lin_ptr;
	int num_threads = 2; // number of threads supported by test
	int bytes_read = 0;
	int total_bytes_to_read = 1024 * 4; // 4KB
	int thread_read_size, thread_read_offset;
	int tile_h;

	if (test->thread_num >= num_threads) {
		UBWCP_LOG(UBWCP_WARN, "Test only supports up to 4 threads, thread id:%d", test->thread_num);
		goto err;
	}

	if (test->test_meta->access_desc != NULL) {
		UBWCP_LOG(UBWCP_WARN, "This test doesn't support overriding the access type");
		goto err;
	}

	ret = init_image(true, test->image_format, test->image_id, &image_lin_raw, &lin_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed linear format:%d id:%d", (int)test->image_format, test->image_id);
		goto err;
	}

	Image_Attrs *image_attrs;
	ret = get_image_attrs(true, (UBWCPLib_Image_Format)test->image_format, test->image_id, &image_attrs);
	if (ret != 0)
		goto free_lin_raw;

	ret = get_tile_w_bytes(test->image_format, &thread_read_size);
	if (ret != 0)
		goto free_lin_raw;

	ret = get_tile_h(test->image_format, &tile_h);
	if (ret != 0)
		goto free_lin_raw;

	if (test->thread_num == 0) {
		// Read from top half of 1st tile
		thread_read_offset = 0;
	} else if (test->thread_num == 1) {
		// Read from bottom half of 1st tile
		thread_read_offset = image_attrs->stride * (tile_h - 1);
	} else {
		UBWCP_LOG(UBWCP_WARN, "test doesn't support more than 2 threads thread num:%d", test->thread_num);
		ret = -EINVAL;
		goto free_lin_raw;
	}

	// Test UBWC-P read
	image_lin_ptr =(char *) mmap(NULL, lin_size, PROT_READ | PROT_WRITE, MAP_SHARED, test->image_fd, 0);
	if (image_lin_ptr == MAP_FAILED) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_WARN, "Failed to mmap linear buffer, this one");
		goto free_lin_raw;
	}

	ret = allocator->CpuSyncStart(test->image_fd, kSyncReadWrite, NULL, NULL);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto free_lin_raw;
	}

	bytes_read = 0;
	while (bytes_read < total_bytes_to_read) {
		cmp = memcmp(image_lin_raw + thread_read_offset, image_lin_ptr + thread_read_offset, thread_read_size);
		if (cmp) {
			if (dump_diff_to_file) {
				dump_image_diff(image_lin_ptr + thread_read_offset, image_lin_raw + thread_read_offset, thread_read_size);
				dump_to_file(image_lin_ptr, lin_size, image_attrs->file_path);
			}
			munmap(image_lin_ptr, lin_size);
			UBWCP_LOG(UBWCP_WARN, "linear img:%d indx:%d read offset:%d read size:%d cmp failed, bytes read:%d",
					test->image_format, test->image_id, bytes_read, thread_read_offset, thread_read_size);
			if (pause_on_failure) {
				UBWCP_LOG(UBWCP_ERR, "Test failed & blocked, press enter to continue:");
				fflush(0);
				getchar();
			}
			ret = -EINVAL;
			goto free_lin_raw;
		}
		bytes_read += thread_read_size;
	}

	ret = allocator->CpuSyncEnd(test->image_fd, kSyncReadWrite, NULL, NULL);
        if (ret != 0) {
                munmap(image_lin_ptr, lin_size);
                goto free_lin_raw;
        }

	munmap(image_lin_ptr, lin_size);

free_lin_raw:
	free(image_lin_raw);

err:
	return ret;
}

/*
 * Tests UBWC-P reading and writting from a shared image by multiple threads
 * with no tiles shared between threads.
 * This test shares an image with up to num_threads threads.
 * It partitions the image into num_thread sections and each thread only reads
 * and writes from its section.
 * The section is intialized using UBWC-P writes.
 * The contents read from each section using UBWC-P are compared against the
 * equivalent section in the golden image.
 * Each section is tile aligned.
 * The data is accessed in a linear byte order.
 */
int share_linear_rw_test_common(struct ubwcp_test *test, int num_threads)
{
	char * image_lin_raw = NULL;
	int lin_size;
	int ret = -EINVAL;
	int cmp;
	char * image_lin_ptr;
	int q_height, n_tile_h, thread_height, thread_read_size, thread_read_offset;
	int bytes_per_pixel;

	if (test->thread_num >= num_threads) {
		UBWCP_LOG(UBWCP_WARN, "Test only supports up to %d threads, thread id:%d", num_threads, test->thread_num);
		goto err;
	}

	if (test->test_meta->access_desc != NULL) {
		UBWCP_LOG(UBWCP_WARN, "This test doesn't support overriding the access type");
		ret = -EINVAL;
		goto err;
	}

	ret = get_lin_y_pixel_bytes((UBWCPLib_Image_Format)test->image_format, &bytes_per_pixel);
	if (ret)
		goto err;

	ret = init_image(true, test->image_format, test->image_id, &image_lin_raw, &lin_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed linear format:%d id:%d", (int)test->image_format, test->image_id);
		goto err;
	}

	Image_Attrs *image_attrs;
	ret = get_image_attrs(true, (UBWCPLib_Image_Format)test->image_format, test->image_id, &image_attrs);
	if (ret != 0)
		goto free_lin_raw;

	if (image_attrs->stride != (image_attrs->w * bytes_per_pixel)) {
		ret = 0;
		UBWCP_LOG(UBWCP_INFO, "share_linear_rw_test_common test doesn't support stride padding format:%d id:%d",
				(int)test->image_format, test->image_id);
		goto free_lin_raw;
	}

	q_height = image_attrs->h / num_threads;
	n_tile_h = q_height / COMMON_TILE_HEIGHT;
	thread_height = n_tile_h * COMMON_TILE_HEIGHT;
	thread_read_size = thread_height * image_attrs->stride;

	thread_read_offset = test->thread_num * thread_read_size;

	// Test UBWC-P read
	image_lin_ptr =(char *) mmap(NULL, lin_size, PROT_READ | PROT_WRITE, MAP_SHARED, test->image_fd, 0);
	if (image_lin_ptr == MAP_FAILED) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_WARN, "Failed to mmap linear buffer");
		goto free_lin_raw;
	}

	//First write the linear contents into the image buffer
	ret = allocator->CpuSyncStart(test->image_fd, kSyncReadWrite, NULL, NULL);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto free_lin_raw;
	}

	memcpy(image_lin_ptr + thread_read_offset, image_lin_raw + thread_read_offset, thread_read_size);

	//Ensure writes are compressed and written into buffer
	ret = allocator->CpuSyncEnd(test->image_fd, kSyncReadWrite, NULL, NULL);
        if (ret != 0) {
                munmap(image_lin_ptr, lin_size);
                goto free_lin_raw;
        }

	// Test UBWC-P read
	// Test that UBWC-P can succesfully uncompress the data in the image buffer
	ret = allocator->CpuSyncStart(test->image_fd, kSyncReadWrite, NULL, NULL);
	if (ret != 0) {
		munmap(image_lin_ptr, lin_size);
		goto free_lin_raw;
	}

	cmp = memcmp(image_lin_raw + thread_read_offset, image_lin_ptr + thread_read_offset, thread_read_size);
	if (cmp) {
		if (dump_diff_to_file) {
			dump_image_diff(image_lin_ptr + thread_read_offset, image_lin_raw + thread_read_offset, thread_read_size);
			dump_to_file(image_lin_ptr, lin_size, image_attrs->file_path);
		}
		munmap(image_lin_ptr, lin_size);
		UBWCP_LOG(UBWCP_WARN, "linear img:%d indx:%d read offset:%d read size:%d cmp failed", test->image_format, test->image_id, thread_read_offset, thread_read_size);
		if (pause_on_failure) {
			UBWCP_LOG(UBWCP_ERR, "Test failed & blocked, press enter to continue:");
			fflush(0);
			getchar();
		}
		ret = -EINVAL;
		goto free_lin_raw;
	}

	ret = allocator->CpuSyncEnd(test->image_fd, kSyncReadWrite, NULL, NULL);
        if (ret != 0) {
                munmap(image_lin_ptr, lin_size);
                goto free_lin_raw;
        }

	munmap(image_lin_ptr, lin_size);

free_lin_raw:
	free(image_lin_raw);

err:
	return ret;
}

int share4_linear_rw_test(struct ubwcp_test *test)
{
	return share_linear_rw_test_common(test, 4);
}

int share8_linear_rw_test(struct ubwcp_test *test)
{
	return share_linear_rw_test_common(test, 8);
}

/*
 * Tests UBWC-P reading and writting from a shared tile in an image shared by multiple threads.
 * This test shares an image with up to 2 threads.
 * It partitions the first tile in the image into 2 sections and each thread only reads
 * and writes to its section.
 * The section is intialized using UBWC-P writes.
 * The contents read from each section using UBWC-P are compared against the
 * equivalent section in the golden image.
 * The data is accessed in a linear byte order.
 */
int share2_linear_same_tile_rw_test(struct ubwcp_test *test)
{
	char * image_lin_raw = NULL;
	int lin_size;
	int ret = -EINVAL;
	int cmp;
	void * session;
	char * image_lin_ptr;
	int num_threads = 2; // number of threads supported by test
	int thread_read_size, thread_read_offset;
	int bytes_written = 0;
	int total_bytes_to_write = 4 * 1024; // 4KB
	int tile_h;

	if (test->thread_num >= num_threads) {
		UBWCP_LOG(UBWCP_WARN, "Test only supports up to 2 threads, thread id:%d", test->thread_num);
		goto err;
	}

	if (test->test_meta->access_desc != NULL) {
		UBWCP_LOG(UBWCP_WARN, "This test doesn't support overriding the access type");
		ret = -EINVAL;
		goto err;
	}

	ret = init_image(true, test->image_format, test->image_id, &image_lin_raw, &lin_size);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "init_image failed linear format:%d id:%d", (int)test->image_format, test->image_id);
		goto err;
	}

	while (bytes_written < total_bytes_to_write) {
		Image_Attrs *image_attrs;
		ret = get_image_attrs(true, (UBWCPLib_Image_Format)test->image_format, test->image_id, &image_attrs);
		if (ret != 0)
			goto free_lin_raw;

		ret = get_tile_w_bytes(test->image_format, &thread_read_size);
		if (ret != 0)
			goto free_lin_raw;

		ret = get_tile_h(test->image_format, &tile_h);
		if (ret != 0)
			goto free_lin_raw;

		if (test->thread_num == 0) {
			// Read from top half of 1st tile
			thread_read_offset = 0;
		} else if (test->thread_num == 1) {
			// Read from bottom half of 1st tile
			thread_read_offset = image_attrs->stride * (tile_h - 1);
		} else {
			UBWCP_LOG(UBWCP_WARN, "test doesn't support more than 2 threads thread num:%d", test->thread_num);
			ret = -EINVAL;
			goto free_lin_raw;
		}

		// Test UBWC-P write
		image_lin_ptr =(char *) mmap(NULL, lin_size, PROT_READ | PROT_WRITE, MAP_SHARED, test->image_fd, 0);
		if (image_lin_ptr == MAP_FAILED) {
			ret = -EINVAL;
			UBWCP_LOG(UBWCP_WARN, "Failed to mmap linear buffer");
			goto free_lin_raw;
		}

		//First write the linear contents into the image buffer
		ret = allocator->CpuSyncStart(test->image_fd, kSyncReadWrite, NULL, NULL);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto free_lin_raw;
		}

		memcpy(image_lin_ptr + thread_read_offset, image_lin_raw + thread_read_offset, thread_read_size);

		//Ensure writes are compressed and written into buffer
		ret = allocator->CpuSyncEnd(test->image_fd, kSyncReadWrite, NULL, NULL);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto free_lin_raw;
		}

		// Test UBWC-P read
		// Test that UBWC-P can succesfully uncompress the data in the image buffer
		ret = allocator->CpuSyncStart(test->image_fd, kSyncReadWrite, NULL, NULL);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto free_lin_raw;
		}

		cmp = memcmp(image_lin_raw + thread_read_offset, image_lin_ptr + thread_read_offset, thread_read_size);
		if (cmp) {
			if (dump_diff_to_file) {
				dump_image_diff(image_lin_ptr + thread_read_offset, image_lin_raw + thread_read_offset, thread_read_size);
				dump_to_file(image_lin_ptr, lin_size, image_attrs->file_path);
			}
			munmap(image_lin_ptr, lin_size);
			UBWCP_LOG(UBWCP_WARN, "linear img:%d indx:%d read offset:%d read size:%d cmp failed", test->image_format,
					test->image_id, thread_read_offset, thread_read_size);
			if (pause_on_failure) {
				UBWCP_LOG(UBWCP_ERR, "Test failed & blocked, press enter to continue:");
				fflush(0);
				getchar();
			}
			ret = -EINVAL;
			goto free_lin_raw;
		}

		ret = allocator->CpuSyncEnd(test->image_fd, kSyncReadWrite, NULL, NULL);
		if (ret != 0) {
			munmap(image_lin_ptr, lin_size);
			goto free_lin_raw;
		}

		munmap(image_lin_ptr, lin_size);

		bytes_written += thread_read_size;
	}

free_lin_raw:
	free(image_lin_raw);

err:
	return ret;
}

int adversarial_align_test(struct ubwcp_test *test)
{
	Image_Attrs *attrs;
	void *session = NULL;
	UBWCPLib_buf_attrs attrsUBWCP;
	int bytes_per_pixel;
	size_t expected_alignment;
	size_t stride_alignment;
	int ret;
	int bad_stride, bad_w;

	ret = get_image_attrs(true, (UBWCPLib_Image_Format)test->image_format,  test->image_id, &attrs);
		if (ret)
		return -EINVAL;

	ret = get_lin_y_pixel_bytes((UBWCPLib_Image_Format)test->image_format, &bytes_per_pixel);
	if (ret)
		return -EINVAL;

	session = UBWCPLib_create_session();
	if (!session) {
		return -EINVAL;
	}

	if ((UBWCPLib_Image_Format)test->image_format == UBWCPLib_LINEAR) {
		// There isn't any bad alignment for Linear
		ret = 0;
		goto done;
	}

	ret = get_expected_stride_alignment((UBWCPLib_Image_Format)test->image_format, &expected_alignment);
	if (ret != 0)
		goto free_session;

	ret = UBWCPLib_get_stride_alignment(session, (UBWCPLib_Image_Format)test->image_format, &stride_alignment);
	if (ret != 0)
		goto free_session;

	if (expected_alignment != stride_alignment) {
		UBWCP_LOG(UBWCP_WARN, "UBWCPLib_get_stride_alignment for format:%d wrong, got val:%zu expected:%zu ",
			  test->image_format, stride_alignment, expected_alignment);
		ret = -EINVAL;
		goto free_session;
	}

	// Create un-supported alignment
	bad_stride = attrs->stride - (stride_alignment / 2);
	bad_w = attrs->w - ((stride_alignment / bytes_per_pixel) / 2);

	ret = UBWCPLib_validate_stride(session, bad_stride, (UBWCPLib_Image_Format)attrs->format, bad_w);
	if (!ret) {
		UBWCP_LOG(UBWCP_ERR, "UBWCPLib_validate_stride wrongly passed for image_format:%d, w:%d stride:%d",
			  attrs->format, bad_w, bad_stride);
		ret = -EINVAL;
		goto free_session;
	}

	init_buf_attrs(&attrsUBWCP, attrs->format, bad_w, attrs->h, bad_stride, attrs->scanlines, attrs->planar_padding);
	ret = UBWCPLib_set_buf_attrs(session, test->image_fd, &attrsUBWCP);
	if (!ret) {
		UBWCP_LOG(UBWCP_ERR, "UBWCPLib_set_buf_attrs wrongly passed for image_format:%d, stride:%d",
				      attrs->format, bad_stride);
		ret = -EINVAL;
		goto free_session;
	}

done:
	UBWCPLib_destroy_session(session);
	return 0;

free_session:
	UBWCPLib_destroy_session(session);
	return ret;
}

/////////////////////////////////////////////////////////
//
// Test framework

static int run_test_id(ubwcp_test * test, int image_format, int id)
{
	int test_err = 0;
	int image_fd;
	int ret = 0;

        ret = get_image_attrs(false, (UBWCPLib_Image_Format)image_format, id, &test->image_comp_attrs);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_ERR, "Failed to get UBWC-P image attrs");
		goto err;
	}

	if (!test->test_meta->alloc_each_iter) {
		ret = get_ubwcp_image(test->image_comp_attrs, image_format, id, &image_fd, test,
				      test->test_meta->perf_test);
		if (ret != 0) {
			UBWCP_LOG(UBWCP_ERR, "Failed to get UBWC-P image");
			goto err;
		}
		test->image_fd = image_fd;
	}

	for (int i = 0; i < test->num_iters; i++)
	{
		if (test->test_meta->alloc_each_iter) {
			ret = get_ubwcp_image(test->image_comp_attrs, image_format, id, &image_fd, test,
					      test->test_meta->perf_test);
			if (ret != 0) {
				UBWCP_LOG(UBWCP_ERR, "Failed to get UBWC-P image");
				goto err;
			}
		}

		test->image_fd = image_fd;
		test->image_format = image_format;
		test->image_id = id;

		int tmp_ret = test->test_meta->func(test);
		if (tmp_ret != 0) {
			UBWCP_LOG(UBWCP_WARN, "Sub test %s FAILED err:%d thread num:%d format:%d image id:%d image:%s", test->test_meta->name, tmp_ret, test->thread_num,  image_format, id, test->image_comp_attrs->file_path);
			if (test_err == 0)
				test_err = tmp_ret;
		} else {
			 UBWCP_LOG(UBWCP_INFO, "Sub test %s PASSED thread num:%d format:%d image id:%d image:%s", test->test_meta->name, test->thread_num, image_format, id, test->image_comp_attrs->file_path);
		}

		if (test->test_meta->alloc_each_iter)
			free_image(test);

	}
	test->err_code = test_err;

	if (!test->test_meta->alloc_each_iter)
		free_image(test);

err:
	return ret;
}

static int get_image_id_from_idx(bool linear, UBWCPLib_Image_Format format, int idx, int *id)
{
	int i;
	int cnt = 0;
	int ret = -EINVAL;
	for (i = 0; i < test_image_list.format_list[format].num_images; i++) {
		if (test_image_list.format_list[format].image_list[i].linear == linear) {
			if (cnt == idx) {
				ret = 0;
				*id = test_image_list.format_list[format].image_list[i].id;
				break;
			} else {
				cnt++;
			}
		}
	}
	if (ret != 0) {
		UBWCP_LOG(UBWCP_ERR, "get_image_id_from_idx fail linear:%d, format:%d, idx:%d", linear, (int)format, idx);
	}

	return ret;
}

static int run_test_plan_multithread_format(ubwcp_test * test, int image_format)
{
	int ret = 0;

	if (test->test_meta->image_id == ALL_IDS) {
		int test_err = 0;

		// divide by 2 since there is a linear and UBWC version of each image
		int num_images = get_num_images(image_format) / 2;

		if (num_images <= 0) {
			ret = -EINVAL;
			UBWCP_LOG(UBWCP_ERR, "No images available for format:%d", image_format);
			goto err;
		}

		for (int i = 0 ; i < num_images; i++) {
			int id;
			ret = get_image_id_from_idx(true, (UBWCPLib_Image_Format)image_format, i, &id);
			if (ret)
				break;

			ret = run_test_id(test, image_format, id);
			if (ret)
				break;

			if (test_err == 0 && test->err_code != 0)
				test_err = test->err_code;
		}
		test->err_code = test_err;
	} else {
		ret = run_test_id(test, image_format, test->test_meta->image_id);
	}

err:
	return ret;
}

static int test_plan_multithread_work(ubwcp_test * test)
{
	int ret = 0;

	if (test->test_meta->image_format == ALL_UBWCP_FORMATS) {
		int test_err = 0;

		for (int i = 0; i < UBWCPLib_NUM_FORMATS; i++) {
			ret = run_test_plan_multithread_format(test, i);
			if (ret)
				break;

			if (test_err == 0 && test->err_code != 0)
				test_err = test->err_code;
		}

		test->err_code = test_err;
	} else {
		ret = run_test_plan_multithread_format(test, test->test_meta->image_format);
	}

	return ret;
}

void *test_plan_multithread_work_fp(void *data)
{
	ubwcp_test * test = (ubwcp_test *)data;
	int ret = test_plan_multithread_work(test);

	if (ret != 0)
		test->err_code = ret;

        if (test->err_code != 0)
		//Make sure other threads don't block waiting for failed thread
		unblock_test_threads(test);

	return NULL;
}
static void report_perf_summarize_results(ubwcp_test_plan * test_plan, ubwcp_perf *perf)
{
	uint64_t read_bw = 0; /* Bytes per ms  */
	uint64_t write_bw = 0; /* Bytes per ms  */
	uint64_t write_tp_bw = 0; /* Bytes per ms  */
	uint64_t read_tp_bw = 0; /* Bytes per ms  */

	if (perf->read_bytes && (perf->read_ns / 1000) > 0)
		read_bw = ((perf->read_bytes * 1000) / (perf->read_ns / 1000));

	if (perf->read_bytes && (perf->read_tp_ns / 1000) > 0)
		read_tp_bw = (((perf->read_bytes * test_plan->num_tests) * 1000) / (perf->read_tp_ns / 1000));

	if (perf->write_bytes && (perf->write_ns / 1000) > 0)
		write_bw = ((perf->write_bytes * 1000) / (perf->write_ns / 1000));

	if (perf->write_bytes && (perf->write_tp_ns / 1000) > 0)
		write_tp_bw = (((perf->write_bytes * test_plan->num_tests) * 1000) / (perf->write_tp_ns / 1000));

	UBWCP_LOG(UBWCP_ERR, "Test Plan %s threads:%d iters:%d Performance:", test_plan->name, test_plan->num_tests, test_plan->num_test_plan_iters );
	UBWCP_LOG(UBWCP_ERR, "\t alloc avg:%llu us", perf->alloc_avg_ns / 1000);
	UBWCP_LOG(UBWCP_ERR, "\t free avg:%llu us", perf->free_avg_ns / 1000);
	UBWCP_LOG(UBWCP_ERR, "\t set buf attrs avg:%llu us", perf->set_buf_attrs_avg_ns / 1000);
	UBWCP_LOG(UBWCP_ERR, "\n\t begin read cpu access avg:%llu us", perf->begin_read_cpu_avg_ns / 1000);
	UBWCP_LOG(UBWCP_ERR, "\t end read cpu access avg:%llu us", perf->end_read_cpu_avg_ns / 1000);
	UBWCP_LOG(UBWCP_ERR, "\t begin write cpu access avg:%llu us", perf->begin_write_cpu_avg_ns / 1000);
	UBWCP_LOG(UBWCP_ERR, "\t end write cpu access avg:%llu us", perf->end_write_cpu_avg_ns / 1000);



	UBWCP_LOG(UBWCP_ERR, "\n\t read test size avg:%llu Bytes", perf->read_bytes);
	UBWCP_LOG(UBWCP_ERR, "\t read test time avg:%llu ns", perf->read_ns);
	UBWCP_LOG(UBWCP_ERR, "\t read test bandwidth avg:%llu MB/s", ((read_bw / 1024) * (1000)) / 1024);
	UBWCP_LOG(UBWCP_ERR, "\t read test plan size avg:%llu Bytes", perf->read_bytes * test_plan->num_tests);
	UBWCP_LOG(UBWCP_ERR, "\t read test plan time avg:%llu ns", perf->read_tp_ns);
	UBWCP_LOG(UBWCP_ERR, "\t read test plan bandwidth avg:%llu MB/s",((read_tp_bw / 1024) * (1000))/ 1024);

	UBWCP_LOG(UBWCP_ERR, "\n\t write test size avg:%llu Bytes", perf->write_bytes);
	UBWCP_LOG(UBWCP_ERR, "\t write test time avg:%llu ns", perf->write_ns);
	UBWCP_LOG(UBWCP_ERR, "\t write test bandwidth avg:%llu MB/s",((write_bw / 1024) * (1000))/ 1024);
	UBWCP_LOG(UBWCP_ERR, "\t write test plan size avg:%llu Bytes", perf->write_bytes * test_plan->num_tests);
	UBWCP_LOG(UBWCP_ERR, "\t write test plan time avg:%llu ns", perf->write_tp_ns);
	UBWCP_LOG(UBWCP_ERR, "\t write test plan bandwidth avg:%llu MB/s",((write_tp_bw / 1024) * (1000))/ 1024);
}

static void report_perf_results(ubwcp_test_plan * test_plan, ubwcp_test *test_list)
{
	ubwcp_perf avg_perf = {0};

	// For test plan bandwith we want the time it took all threads in the test plan to complete
	// so don't sum or average.
	avg_perf.read_tp_ns = test_list[0].perf.read_tp_ns;
	avg_perf.write_tp_ns = test_list[0].perf.write_tp_ns;

	for (int i = 0; i < test_plan->num_tests; i++) {
		avg_perf.alloc_avg_ns += test_list[i].perf.alloc_avg_ns;
		avg_perf.free_avg_ns += test_list[i].perf.free_avg_ns;
		avg_perf.set_buf_attrs_avg_ns += test_list[i].perf.set_buf_attrs_avg_ns;
		avg_perf.begin_read_cpu_avg_ns += test_list[i].perf.begin_read_cpu_avg_ns;
		avg_perf.begin_write_cpu_avg_ns += test_list[i].perf.begin_write_cpu_avg_ns;
		avg_perf.end_read_cpu_avg_ns += test_list[i].perf.end_read_cpu_avg_ns;
		avg_perf.end_write_cpu_avg_ns += test_list[i].perf.end_write_cpu_avg_ns;
		avg_perf.read_ns += test_list[i].perf.read_ns;
		avg_perf.read_bytes += test_list[i].perf.read_bytes;
		avg_perf.write_ns += test_list[i].perf.write_ns;
		avg_perf.write_bytes += test_list[i].perf.write_bytes;
	}

	// Don't average the time/bytes read/written for test plan bandwidth
	avg_perf.alloc_avg_ns /= test_plan->num_tests;
	avg_perf.free_avg_ns /= test_plan->num_tests;
	avg_perf.set_buf_attrs_avg_ns /= test_plan->num_tests;
	avg_perf.begin_read_cpu_avg_ns /= test_plan->num_tests;
	avg_perf.end_read_cpu_avg_ns /= test_plan->num_tests;
	avg_perf.begin_write_cpu_avg_ns /= test_plan->num_tests;
	avg_perf.end_write_cpu_avg_ns /= test_plan->num_tests;
	avg_perf.read_ns /= test_plan->num_tests;
	avg_perf.read_bytes /= test_plan->num_tests;
	avg_perf.write_ns /= test_plan->num_tests;
	avg_perf.write_bytes /= test_plan->num_tests;

	report_perf_summarize_results(test_plan, &avg_perf);
}

static int report_results(ubwcp_test_plan * test_plan, ubwcp_test *test_list, int ret)
{
	int i;
	int test_err = 0;
	char * access_type_str;
	int access_type_id;

	if (ret == 0) {
		for (i = 0; i < test_plan->num_tests; i++) {
			if (test_list[i].err_code != 0) {
				UBWCP_LOG(UBWCP_WARN, "Test %s FAILED err:%d", test_plan->test_list[i].name, test_list[i].err_code);

				if (test_err == 0)
					test_err = test_list[i].err_code;
			} else {
				UBWCP_LOG(UBWCP_INFO, "Test %s PASSED", test_plan->test_list[i].name);
			}
		}
		if (test_err == 0 && test_plan->test_list[0].perf_test) {
			report_perf_results(test_plan, test_list);
		}

		UBWCP_LOG(UBWCP_ERR, "Test plan %s: num_test:%d num_tp_iters:%d num_iters:%d", test_plan->name, test_plan->num_tests, test_plan->num_test_plan_iters, test_plan->num_test_iters);

		if (test_plan->test_list[0].access_desc == NULL) {
			access_type_str = BYTE_LINEAR_PLANE_ACCESS.name;
			access_type_id = BYTE_LINEAR_PLANE_FUNC_TYPE;
		} else {
			access_type_str = test_plan->test_list[0].access_desc->name;
			access_type_id = test_plan->test_list[0].access_desc->func_type;
		}

		UBWCP_LOG(UBWCP_ERR, "Test[0] %s details: format:%d id:%d access type:%s",
				test_plan->test_list[0].name, test_plan->test_list[0].image_format, test_plan->test_list[0].image_id, access_type_str);

		UBWCP_LOG(UBWCP_INFO, "Params: -num_tp_iters:%d -num_iters=%d -num_threads=%d -image_format=%d image_id=%d -access_type=%d",
				test_plan->num_test_plan_iters, test_plan->num_test_iters, test_plan->num_tests, test_plan->test_list[0].image_format, test_plan->test_list[0].image_id, access_type_id);

		if (test_err != 0) {
			UBWCP_LOG(UBWCP_ERR, "Test Plan %s FAILED err:%d", test_plan->name, test_err);
		} else {
			UBWCP_LOG(UBWCP_ERR, "Test Plan %s PASSED", test_plan->name);
		}
	} else {
		UBWCP_LOG(UBWCP_ERR, "Error running tests err:%d", ret);
	}

	return test_err;
}

static int run_test_plan_multithread(ubwcp_test_plan * test_plan, int *test_err)
{
	int i;
	int ret;
	ubwcp_test *test_list = (ubwcp_test *) malloc(sizeof(ubwcp_test) * test_plan->num_tests);
	int num_test_plan_iters = test_plan->num_test_plan_iters;

	if (num_test_plan_iters == 0)
		num_test_plan_iters = 1; // Handle default value

	if (!test_list)
		return -ENOMEM;

	if (test_plan->num_tests == 0)
		return -EINVAL;

	if (test_plan->test_list[0].perf_test && test_plan->num_test_iters > 1) {
		UBWCP_LOG(UBWCP_WARN, "Error: perf test has num_test_iters > 1, use num_test_plan_iters instead");
		return -EINVAL;
	}

	for (int k = 0; k < num_test_plan_iters; k++) {
		if (test_plan->num_tests > 1) {
			int thread_num = 0;
			ret = 0;
			pthread_t * thread_list = new pthread_t[test_plan->num_tests];
			atomic_int write_st_wait_cnt;
			atomic_int read_st_wait_cnt;
			atomic_int write_end_wait_cnt;
			atomic_int read_end_wait_cnt;

			atomic_init(&write_st_wait_cnt, test_plan->num_tests);
			atomic_init(&read_st_wait_cnt, test_plan->num_tests);
			atomic_init(&write_end_wait_cnt, test_plan->num_tests);
			atomic_init(&read_end_wait_cnt, test_plan->num_tests);

			if (!thread_list) {
				ret = -ENOMEM;
				goto free_test_list;
			}

			if (test_plan->test_list[0].perf_test) {
				ret = sem_init(&test_plan->write_st_sem, 0, 0);
				if (ret != 0)
					goto free_test_list;

				ret = sem_init(&test_plan->write_end_sem, 0, 0);
				if (ret != 0)
					goto free_test_list;

				ret = sem_init(&test_plan->read_st_sem, 0, 0);
				if (ret != 0)
					goto free_test_list;

				ret = sem_init(&test_plan->read_end_sem, 0, 0);
				if (ret != 0)
					goto free_test_list;
			}

			for (i = 0; i < test_plan->num_tests; i++) {
				int tmp_ret;
				ubwcp_test_meta *test_meta = &test_plan->test_list[i];
				ubwcp_test *test = &test_list[i];

				test->test_meta = test_meta;
				test->thread_num = i;
				test->num_iters = test_plan->num_test_iters;
				if (test->test_meta->perf_test) {
					test->write_st_wait_cnt = &write_st_wait_cnt;
					test->write_st_sem = &test_plan->write_st_sem;
					test->write_end_wait_cnt = &write_end_wait_cnt;
					test->write_end_sem = &test_plan->write_end_sem;
					test->read_st_wait_cnt = &read_st_wait_cnt;
					test->read_end_wait_cnt = &read_end_wait_cnt;
					test->read_st_sem = &test_plan->read_st_sem;
					test->read_end_sem = &test_plan->read_end_sem;
				}
				tmp_ret = pthread_create( &thread_list[i], NULL, test_plan_multithread_work_fp , (void*) test);
				if (tmp_ret != 0) {
					UBWCP_LOG(UBWCP_WARN, "Failure to create pthread err:%d", tmp_ret);
					test->err_code = tmp_ret;
					ret = tmp_ret;
					break;
				} else {
					thread_num = i + 1;
				}
			}

			for (i = 0; i < thread_num; i++) {
				pthread_join(thread_list[i], NULL);
			}

			free(thread_list);
		} else {
			ubwcp_test *test = &test_list[0];
			ubwcp_test_meta *test_meta = &(test_plan->test_list[0]);

			test->test_meta = test_meta;
			test->thread_num = 0;
			test->num_iters = test_plan->num_test_iters;
			ret = test_plan_multithread_work(test);
		}

		if (test_plan->test_list[0].perf_test) {
			uint64_t earliest_write_st = 0;
			uint64_t latest_write_end = 0;
			uint64_t earliest_read_st = 0;
			uint64_t latest_read_end = 0;

			// For mutithreaded we want to use the total time it took to complete all the reads
			// and writes for this test plan run.
			for (i = 0; i < test_plan->num_tests; i++) {
				ubwcp_test *test = &test_list[i];
				if (earliest_write_st == 0 || earliest_write_st > test->perf.write_st_ns)
					earliest_write_st = test->perf.write_st_ns;

				if (latest_write_end == 0 || latest_write_end < test->perf.write_end_ns)
					latest_write_end = test->perf.write_end_ns;

				if (earliest_read_st == 0 || earliest_read_st > test->perf.read_st_ns)
					earliest_read_st = test->perf.read_st_ns;

				if (latest_read_end == 0 || latest_read_end < test->perf.read_end_ns)
					latest_read_end = test->perf.read_end_ns;
			}

			for (i = 0; i < test_plan->num_tests; i++) {
				ubwcp_test *test = &test_list[i];
				test->perf.write_tp_ns += latest_write_end - earliest_write_st;
				test->perf.read_tp_ns += latest_read_end - earliest_read_st;
			}
		}
	}
	if (test_plan->test_list[0].perf_test) {
		for (i = 0; i < test_plan->num_tests; i++) {
			ubwcp_test *test = &test_list[i];
			test->perf.alloc_avg_ns /= num_test_plan_iters;
			test->perf.free_avg_ns /= num_test_plan_iters;
			test->perf.set_buf_attrs_avg_ns /= num_test_plan_iters;
			if (test->perf.begin_read_cpu_num)
				test->perf.begin_read_cpu_avg_ns /= test->perf.begin_read_cpu_num;
			if (test->perf.begin_write_cpu_num)
				test->perf.begin_write_cpu_avg_ns /= test->perf.begin_write_cpu_num;
			if (test->perf.end_read_cpu_num)
				test->perf.end_read_cpu_avg_ns /= test->perf.end_read_cpu_num;
			if (test->perf.end_write_cpu_num)
				test->perf.end_write_cpu_avg_ns /= test->perf.end_write_cpu_num;
			test->perf.read_tp_ns /= num_test_plan_iters;
			test->perf.read_ns /= num_test_plan_iters;
			test->perf.read_bytes /= num_test_plan_iters;
			test->perf.write_tp_ns /= num_test_plan_iters;
			test->perf.write_ns /= num_test_plan_iters;
			test->perf.write_bytes /= num_test_plan_iters;
		}
	}

	*test_err = report_results(test_plan, test_list, ret);

free_test_list:
	free(test_list);

	return ret;

}

static int test_plan_multithread_share_image_work(ubwcp_test * test)
{
	int test_err = 0;

	for (int i = 0; i < test->num_iters; i++)
	{
		int tmp_ret = test->test_meta->func(test);
		if (tmp_ret != 0) {
			UBWCP_LOG(UBWCP_WARN, "Sub test %s FAILED err:%d thread num:%d format:%d image id:%d image:%s", test->test_meta->name, tmp_ret, test->thread_num,  test->image_format, test->image_id, test->image_comp_attrs->file_path);
			if (test_err == 0)
				test_err = tmp_ret;
		} else {
			 UBWCP_LOG(UBWCP_INFO, "Sub test %s PASSED thread num:%d format:%d image id:%d image:%s", test->test_meta->name, test->thread_num, test->image_format, test->image_id, test->image_comp_attrs->file_path);
		}
	}

	return test_err;
}

void *test_plan_multithread_shared_image_work_fp(void *data)
{
	ubwcp_test * test = (ubwcp_test *)data;
	int ret = test_plan_multithread_share_image_work(test);

	if (ret != 0)
		test->err_code = ret;

	if (test->err_code != 0)
		//Make sure other threads don't block waiting for failed thread
		unblock_test_threads(test);

	return NULL;
}

static int run_test_plan_multithread_share_image_id(ubwcp_test_plan * test_plan, ubwcp_test * test_list, int image_format, int image_idx)
{
	int test_err = 0;
	int image_fd;
	int ret = 0;
	int i;
	int thread_num = 0;
	pthread_t * thread_list;

	ubwcp_test * test1 = &test_list[0];
        ret = get_image_attrs(false, (UBWCPLib_Image_Format)image_format, image_idx, &test1->image_comp_attrs);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_ERR, "Failed to get UBWC-P image attrs");
		goto err;
	}

	ret = get_ubwcp_image(test1->image_comp_attrs, image_format, image_idx, &image_fd, NULL, false);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_ERR, "Failed to get UBWC-P image");
		goto err;
	}

	if (test_plan->pre_init_image) {
		ret = pre_init_image(image_fd, image_format, image_idx);
		if (ret != 0) {
			UBWCP_LOG(UBWCP_ERR, "Failed to pre initialize UBWC-P image");
			goto err;
		}
	}

	thread_list = new pthread_t[test_plan->num_tests];
	if (!thread_list) {
		ret = -ENOMEM;
		goto free_image;
	}

	for (i = 0; i < test_plan->num_tests; i++) {
		int tmp_ret;
		ubwcp_test_meta *test_meta = &test_plan->test_list[i];
		ubwcp_test *test = &test_list[i];

		tmp_ret = get_image_attrs(false, (UBWCPLib_Image_Format)image_format, image_idx, &test->image_comp_attrs);
		if (tmp_ret != 0) {
			UBWCP_LOG(UBWCP_ERR, "Failed to get UBWC-P image attrs");
			test->err_code = tmp_ret;
			ret = tmp_ret;
			break;
		}
		test->thread_num = i;
		test->image_fd = image_fd;
		test->image_format = image_format;
		test->image_id = image_idx;

		tmp_ret = pthread_create( &thread_list[i], NULL, test_plan_multithread_shared_image_work_fp , (void*) test);
		if (tmp_ret != 0) {
			UBWCP_LOG(UBWCP_WARN, "Failure to create pthread err:%d", tmp_ret);
			test->err_code = tmp_ret;
			ret = tmp_ret;
			break;
		} else {
			thread_num = i + 1;
		}
	}

	for (i = 0; i < thread_num; i++) {
		pthread_join(thread_list[i], NULL);
	}

	free(thread_list);

free_image:
	close(image_fd);

err:
	return ret;
}

static int run_test_plan_multithread_shared_image_format(ubwcp_test_plan * test_plan, ubwcp_test * test_list, int image_format)
{
	int ret = 0;
	ubwcp_test_meta *test_meta = &(test_plan->test_list[0]);

	if (test_meta->image_id == ALL_IDS) {
		int test_err = 0;

		// divide by 2 since there is a linear and UBWC version of each image
		int num_images = get_num_images(image_format) / 2;

		if (num_images <= 0) {
			ret = -EINVAL;
			UBWCP_LOG(UBWCP_ERR, "No images available for format:%d", image_format);
			goto err;
		}

		for (int i = 0 ; i < num_images; i++) {
			int id;
			ret = get_image_id_from_idx(true, (UBWCPLib_Image_Format)image_format, i, &id);
			if (ret)
				break;

			ret = run_test_plan_multithread_share_image_id(test_plan, test_list, image_format, id);
			if (ret)
				break;
		}
	} else {
		ret = run_test_plan_multithread_share_image_id(test_plan, test_list, image_format, test_meta->image_id);
	}

err:
	return ret;
}

static int run_test_plan_shared_image_multithread(ubwcp_test_plan * test_plan, int *test_err)
{
	int i;
	int ret;
	ubwcp_test *test_list;

	if (test_plan->num_tests <= 1) {
		UBWCP_LOG(UBWCP_ERR, "Shared image test plans must have more than one test");
		return -EINVAL;
	}

	ubwcp_test_meta *test_meta = &(test_plan->test_list[0]);

	for (i = 1; i < test_plan->num_tests; i++) {
		ubwcp_test_meta *test_meta_tmp = &test_plan->test_list[i];
		if (strncmp(test_meta->name, test_meta_tmp->name, strlen(test_meta->name)) != 0) {
			UBWCP_LOG(UBWCP_ERR, "Shared image test plans must have same tests");
			return -EINVAL;
		}
	}


	test_list = (ubwcp_test *) malloc(sizeof(ubwcp_test) * test_plan->num_tests);
	if (!test_list)
		return -ENOMEM;

	for (i = 0; i < test_plan->num_tests; i++) {
		int tmp_ret;
		ubwcp_test_meta *test_meta_tmp = &test_plan->test_list[i];
		ubwcp_test *test = &test_list[i];

		test->test_meta = test_meta_tmp;
		test->num_iters = test_plan->num_test_iters;
	}

	// test_meta is the same for all tests in a shared image test plan
	if (test_meta->image_format == ALL_UBWCP_FORMATS) {
		for (int i = 0; i < UBWCPLib_NUM_FORMATS; i++) {
			ret = run_test_plan_multithread_shared_image_format(test_plan, test_list, i);
			if (ret)
				break;
		}
	} else {
		ret = run_test_plan_multithread_shared_image_format(test_plan, test_list, test_meta->image_format);
	}

	*test_err = report_results(test_plan, test_list, ret);

	free(test_list);
	return ret;
}

static int run_test_plan(ubwcp_test_plan *test_plan, int *test_err)
{
	int ret;

	if (test_plan->share_image) {
		ret = run_test_plan_shared_image_multithread(test_plan, test_err);
	} else {
		ret = run_test_plan_multithread(test_plan, test_err);
	}

	return ret;
}

/////////////////////////////////////////////////////////
// Image file parsing

static int parse_image_format(const char *str, int *format)
{
	int ret = 0;

	if (strcmp(str, "LINEAR") == 0) {
		*format = UBWCPLib_LINEAR;
	} else if (strcmp(str, "RGBA8888") == 0) {
		*format = UBWCPLib_RGBA8888;
	} else if (strcmp(str, "NV12") == 0) {
		*format = UBWCPLib_NV12;
	} else if (strcmp(str, "NV12Y") == 0) {
		*format = UBWCPLib_NV12_Y;
	} else if (strcmp(str, "NV12UV") == 0) {
		*format = UBWCPLib_NV12_UV;
	} else if (strcmp(str, "NV124R") == 0) {
		*format = UBWCPLib_NV124R;
	} else if (strcmp(str, "NV124R_Y") == 0) {
		*format = UBWCPLib_NV124R_Y;
	} else if (strcmp(str, "NV124RUV") == 0) {
		*format = UBWCPLib_NV124R_UV;
	} else if (strcmp(str, "NV21") == 0) {
		*format = UBWCPLib_NV12;
	} else if (strcmp(str, "NV21Y") == 0) {
		*format = UBWCPLib_NV12_Y;
	} else if (strcmp(str, "NV21UV") == 0) {
		*format = UBWCPLib_NV12_UV;
	} else if (strcmp(str, "NV214R") == 0) {
		*format = UBWCPLib_NV124R;
	} else if (strcmp(str, "NV214R_Y") == 0) {
		*format = UBWCPLib_NV124R_Y;
	} else if (strcmp(str, "NV214RUV") == 0) {
		*format = UBWCPLib_NV124R_UV;
	} else if (strcmp(str, "TP10") == 0) {
		*format = UBWCPLib_TP10;
	} else if (strcmp(str, "TP10Y") == 0) {
		*format = UBWCPLib_TP10_Y;
	} else if (strcmp(str, "TP10UV") == 0) {
		*format = UBWCPLib_TP10_UV;
	} else if (strcmp(str, "P010") == 0) {
		*format = UBWCPLib_P010;
	} else if (strcmp(str, "P010Y") == 0) {
		*format = UBWCPLib_P010_Y;
	} else if (strcmp(str, "P010UV") == 0) {
		*format = UBWCPLib_P010_UV;
	} else if (strcmp(str, "P016") == 0) {
		*format = UBWCPLib_P016;
	} else if (strcmp(str, "P016Y") == 0) {
		*format = UBWCPLib_P016_Y;
	} else if (strcmp(str, "P016UV") == 0) {
		*format = UBWCPLib_P016_UV;
	} else {
		ret = -EINVAL;
	}

	return ret;
}


//str == "name[val]"
static int parse_bracket_val(const char * str, const char *name, int *val)
{
	char * strp_base;
	char * strp;
	char * token;
	int ret = 0;

	if (strlen(str) < strlen(name) + 3) {
		ret = -EINVAL;
		goto err;
	}

	strp_base = strdup(str);
	if (!strp_base) {
		ret = -EINVAL;
		goto err;
	}
	strp = strp_base;

	strp += strlen(name);
	strp++;
	token = strsep(&strp, "]");
	if (!token) {
		ret = -ENOMEM;
		goto free_strp_base;
	}
	*val = atoi(token);

free_strp_base:
	free(strp_base);

err:
	return ret;
}

static int parse_file_name(char * fname, Image_Attrs * attrs)
{
	//Example of properly constructed file name
	//image_1_v1_w[1440]_h[1080]_stride[1536]_scanlines[1088]_UBWC_NV12
	//image_1_v1_w[1440]_h[1080]_stride[1536]_scanlines[1088]_LINEAR_NV12
	int idx = 0;
	int ret = 0;
	char * str_base = strdup(fname);
	char * str;
	int cmp;
	char * token;
	int version;
	int stride_bytes = 0;

	if (!str_base)
		return -ENOMEM;

	str = str_base;

	while ((token = strsep(&str, "_"))) {
		switch(idx) {
		case 0:
			//"image"
			cmp = strcmp(token, "image");
		        if (cmp != 0) {
				ret = -EINVAL;
				goto err;
			}
			break;
		case 1:
			//index
			attrs->id = atoi(token);
			break;
		case 2:
			//version
			token++;
			version = atoi(token);
			if (version != 1) {
				ret = -EINVAL;
				goto err;
			}
			break;
		case 3:
			//width
			ret = parse_bracket_val(token, "w", &attrs->w);
			if (ret)
				goto err;
			break;
		case 4:
			//height
			ret = parse_bracket_val(token, "h", &attrs->h);
			if (ret)
				goto err;
			break;
		case 5:
			//stride
			ret = parse_bracket_val(token, "stride", &attrs->stride);
			if (ret)
				goto err;
			break;
		case 6:
			//scanline;
			ret = parse_bracket_val(token, "scanlines", &attrs->scanlines);
			if (ret)
				goto err;
			break;
		case 7:
			//check for optional planar padding
			ret = parse_bracket_val(token, "planpad", &attrs->planar_padding);
			if (ret == 0)
				token = strsep(&str, "_");
			ret = 0;

			//LINEAR vs UBWC
			if (strcmp(token, "UBWC") == 0) {
				attrs->linear = false;
			} else if (strcmp(token, "LINEAR") == 0) {
				attrs->linear = true;
			} else {
				ret = -EINVAL;
				UBWCP_LOG(UBWCP_WARN, "Failed token :%s:", token);
				goto err;
			}
			break;
		case 8:
			//Image format
			ret = parse_image_format(token, (int *)&attrs->format);
			break;
		default:
			ret = -EINVAL;
			idx++;
			goto err;
		}

		idx++;
	}

	if (idx != 9) {
		ret = -EINVAL;
		goto err;
	}

	//convert stride pixel to stride bytes
	ret = get_stride_bytes((UBWCPLib_Image_Format)attrs->format, attrs->stride, &stride_bytes);
	if (ret != 0) {
		UBWCP_LOG(UBWCP_WARN, "Failed to convet stride to bytes");
		goto err;
	}
	attrs->stride = stride_bytes;

	free(str_base);
	return 0;
err:
	UBWCP_LOG(UBWCP_ERR, "File parsing error idx:%d str:%s", idx, fname);
	free(str_base);
	return ret;

}

static int parse_image_dir(struct dirent *dir, const char * base_path, UBWCP_image_format_list *format_list, UBWCPLib_Image_Format format)
{
	// Count num files
	int num_files = 0;
	struct dirent *file;
	int ret = 0;
	int idx;
	size_t path_size = strlen(base_path) + strlen(dir->d_name) + 2;

	char *path = (char *) malloc(path_size);
	if (!path)
		return -ENOMEM;

	snprintf(path, path_size, "%s/%s", base_path, dir->d_name);

	DIR * d = opendir(path);
	if (!d) {
		ret = -EINVAL;
		UBWCP_LOG(UBWCP_ERR, "Could not open dir:%s", path);
		goto free_path;
	}

	while ((file = readdir(d)) != NULL) {
		if (file->d_type == DT_REG) {
			struct Image_Attrs tmp_attrs;

			if (parse_file_name(file->d_name, &tmp_attrs) == 0)
				num_files++;
			else
				UBWCP_LOG(UBWCP_ERR, "Failure to parse file name:%s", file->d_name);
		}
	}
	format_list->format = format;
	format_list->image_list = (Image_Attrs *) malloc(sizeof(Image_Attrs) * num_files);
	if (!format_list->image_list) {
		ret = -ENOMEM;
		UBWCP_LOG(UBWCP_ERR, "Failed to allocate format_list->image_list");
		goto free_path;
	}

	rewinddir(d);
	idx = 0;
	while ((file = readdir(d)) != NULL && idx < num_files) {
		if (file->d_type == DT_REG) {
			Image_Attrs * attrs = &format_list->image_list[idx];
			size_t rel_path_size = strlen(path) + strlen(file->d_name) + 2;
			char *rel_path = (char *) malloc(rel_path_size);
			if (!rel_path) {
				ret = -ENOMEM;
				UBWCP_LOG(UBWCP_ERR, "Failed to allocate rel_path");
				goto free_path;
			}

			snprintf(rel_path, rel_path_size, "%s/%s", path, file->d_name);
			attrs->file_path = rel_path;

			int ret = parse_file_name(file->d_name, attrs);
			if (ret != 0) {
				UBWCP_LOG(UBWCP_ERR, "Failure to parse file:%s", file->d_name);
				attrs->file_path = NULL;
				free(rel_path);
				continue;
			}

			struct stat st;
			stat(rel_path, &st);
			attrs->file_size = st.st_size;
			if (attrs->file_size == 0) {
				UBWCP_LOG(UBWCP_ERR, "Error: file size is 0, file:", file->d_name);
				attrs->file_path = NULL;
				free(rel_path);
				continue;
			}
			idx++;
		}
	}
	format_list->num_images = idx;

	closedir(d);
	if (idx == 0) {
		ret = -EINVAL;
	}

free_path:
	return ret;
}

static int parse_available_images(const char * base_path, UBWCP_image_list * image_set)
{
	struct dirent * dir;
	int ret = -EINVAL;
	DIR * d = opendir(base_path);

	if (d == NULL) {
		return -EINVAL;
	}

	while ((dir = readdir(d)) != NULL) {
		if(dir->d_type == DT_DIR &&
		   strcmp(dir->d_name,".") != 0 &&
		   strcmp(dir->d_name,"..") != 0 ) {
			int format;

			ret = parse_image_format(dir->d_name, &format);
			if (ret != 0) {
				UBWCP_LOG(UBWCP_ERR, "Error: Unsuported image folder:%s, err:%d", dir->d_name, ret);
				continue;
			}

			ret = parse_image_dir(dir, base_path, &image_set->format_list[format], (UBWCPLib_Image_Format)format);
			if (ret != 0) {
				UBWCP_LOG(UBWCP_ERR, "Error parsing image folder:%s, err:%d", dir->d_name, ret);
			}
		}
	}
	closedir(d);
	return ret;
}

/////////////////////////////////////////////////////////
// UBWC-P Tests

// RW Tests
ubwcp_test_meta COMPLETE_RW_ID1_NV12_TEST = {
	.name = "Complete RW NV12 ID1 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = 1,
};

ubwcp_test_meta CUSTOM_COMPLETE_RW_TEST = {
	.name = "Custom Complete RW Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = 1,
};

ubwcp_test_meta COMPLETE_RW_ALL_NV12_TEST = {
	.name = "Complete RW NV12 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS
};

ubwcp_test_meta WHOLE_RW_ALL_NV12_TEST = {
	.name = "Whole RW NV12 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS,
	.access_desc = &BYTE_LINEAR_WHOLE_ACCESS
};

ubwcp_test_meta COMPLETE_RW_ALL_RGBA8888_TEST = {
	.name = "Complete RW RGBA8888 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_RGBA8888,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RW_ALL_NV124R_TEST = {
	.name = "Complete RW NV124R Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_NV124R,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RW_ALL_TP10_TEST = {
	.name = "Complete RW TP10 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_TP10,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RW_ALL_P010_TEST = {
	.name = "Complete RW P010 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_P010,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RW_ALL_P016_TEST = {
	.name = "Complete RW P016 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_P016,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RW_ALL_IMAGE_TEST = {
	.name = "Complete RW all images Test",
	.func = complete_rw_test,
	.image_format = ALL_UBWCP_FORMATS,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RW_ALL_LINEAR_TEST = {
	.name = "Complete RW LINEAR Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_LINEAR,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RW_ID9_PERF_NV12_TEST = {
	.name = "Complete RW Perf NV12 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = 9,
	.perf_test = true,
	.access_desc = &BYTE_LINEAR_WHOLE_ACCESS
};

ubwcp_test_meta COMPLETE_RW_ID9_PERF_LINEAR_TEST = {
	.name = "Complete RW Perf LINEAR Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_LINEAR,
	.image_id = 9,
	.perf_test = true,
	.access_desc = &BYTE_LINEAR_WHOLE_ACCESS
};

ubwcp_test_meta COMPLETE_RW_2GB_ULAPA_RANGE_NV12_TEST = {
	.name = "Complete RW 2GB ULA PA Range NV12 Test",
	.func = ulapa_range_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = 9,
	.priv1 = 0x80000000
};

ubwcp_test_meta COMPLETE_RW_227_DESC_NV12_TEST = {
        .name = "Complete RW 227 DESCRIPTOR NV12 Test",
        .func = ulapa_range_rw_test,
        .image_format = UBWCPLib_NV12,
        .image_id = 1,
        .priv1 = 0x10000000
};

ubwcp_test_meta COMPLETE_RW_ALL_VERT_COL4B_NV12_TEST = {
	.name = "Complete RW Vertical Col 4B NV12 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS,
	.access_desc = &BYTE_VERTICAL_COL4B_PLANE_ACCESS
};

ubwcp_test_meta COMPLETE_RW_ALL_VERT_COL4B_TP10_TEST = {
	.name = "Complete RW Vertical Col 4B TP10 Test",
	.func = complete_rw_test,
	.image_format = UBWCPLib_TP10,
	.image_id = ALL_IDS,
	.access_desc = &BYTE_VERTICAL_COL4B_PLANE_ACCESS
};

// RO Tests
ubwcp_test_meta CUSTOM_COMPLETE_RO_TEST = {
	.name = "Custom Complete RO Test",
	.func = complete_ro_test,
	.image_format = UBWCPLib_NV12,
	.image_id = 1,
};

ubwcp_test_meta COMPLETE_RO_ALL_NV12_TEST = {
	.name = "Complete RO NV12 Test",
	.func = complete_ro_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RO_ALL_RGBA8888_TEST = {
	.name = "Complete RO RGBA8888 Test",
	.func = complete_ro_test,
	.image_format = UBWCPLib_RGBA8888,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RO_ALL_TP10_TEST = {
	.name = "Complete RO TP10 Test",
	.func = complete_ro_test,
	.image_format = UBWCPLib_TP10,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RO_ALL_P010_TEST = {
	.name = "Complete RO P010 Test",
	.func = complete_ro_test,
	.image_format = UBWCPLib_P010,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RO_ALL_NV124R_TEST = {
        .name = "Complete RO NV124R Test",
        .func = complete_ro_test,
        .image_format = UBWCPLib_NV124R,
        .image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RO_ALL_P016_TEST = {
        .name = "Complete RO P016 Test",
        .func = complete_ro_test,
        .image_format = UBWCPLib_P016,
        .image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RO_ALL_LINEAR_TEST = {
	.name = "Complete RO LINEAR Test",
	.func = complete_ro_test,
	.image_format = UBWCPLib_LINEAR,
	.image_id = ALL_IDS
};

ubwcp_test_meta COMPLETE_RO_ID9_PERF_NV12_TEST = {
	.name = "Complete RO Perf NV12 Test",
	.func = complete_ro_test,
	.image_format = UBWCPLib_NV12,
	.image_id = 9,
	.perf_test = true,
	.access_desc = &BYTE_LINEAR_WHOLE_ACCESS
};

ubwcp_test_meta COMPLETE_RO_ID9_PERF_LINEAR_TEST = {
	.name = "Complete RO Perf LINEAR Test",
	.func = complete_ro_test,
	.image_format = UBWCPLib_LINEAR,
	.image_id = 9,
	.perf_test = true,
	.access_desc = &BYTE_LINEAR_WHOLE_ACCESS
};

ubwcp_test_meta COMPLETE_RO_2GB_ULAPA_RANGE_NV12_TEST = {
	.name = "Complete RO 2GB ULA PA Range NV12 Test",
	.func = ulapa_range_ro_test,
	.image_format = UBWCPLib_NV12,
	.image_id = 9,
	.priv1 = 0x80000000
};

ubwcp_test_meta COMPLETE_RO_227_DESC_NV12_TEST = {
        .name = "Complete RO 227 DESCRIPTOR NV12 Test",
        .func = ulapa_range_ro_test,
        .image_format = UBWCPLib_NV12,
        .image_id = 1,
        .priv1 = 0x10000000
};

// Shared image diff tile RO Tests
ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV12_TEST = {
	.name = "Shared4 different tile RO linear NV12 Test",
	.func = share4_linear_ro_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RO_ALL_RGBA8888_TEST = {
	.name = "Shared4 different tile RO linear RGBA8888 Test",
	.func = share4_linear_ro_test,
	.image_format = UBWCPLib_RGBA8888,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RO_ALL_TP10_TEST = {
	.name = "Shared4 different tile RO linear TP10 Test",
	.func = share4_linear_ro_test,
	.image_format = UBWCPLib_TP10,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RO_ALL_P010_TEST = {
	.name = "Shared4 different tile RO linear P010 Test",
	.func = share4_linear_ro_test,
	.image_format = UBWCPLib_P010,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV124R_TEST = {
	.name = "Shared4 different tile RO linear NV124R Test",
	.func = share4_linear_ro_test,
	.image_format = UBWCPLib_NV124R,
	.image_id = ALL_IDS
};

// Shared image diff tile RW Tests
ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST = {
	.name = "Shared4 different tile RW linear NV12 Test",
	.func = share4_linear_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST = {
	.name = "Shared4 different tile RW linear RGBA8888 Test",
	.func = share4_linear_rw_test,
	.image_format = UBWCPLib_RGBA8888,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST = {
	.name = "Shared4 different tile RW linear TP10 Test",
	.func = share4_linear_rw_test,
	.image_format = UBWCPLib_TP10,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RW_ALL_P010_TEST = {
	.name = "Shared4 different tile RW linear P010 Test",
	.func = share4_linear_rw_test,
	.image_format = UBWCPLib_P010,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST = {
	.name = "Shared4 different tile RW linear NV124R Test",
	.func = share4_linear_rw_test,
	.image_format = UBWCPLib_NV124R,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST = {
	.name = "Shared8 different tile RW linear NV12 Test",
	.func = share8_linear_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST = {
	.name = "Shared8 different tile RW linear RGBA8888 Test",
	.func = share8_linear_rw_test,
	.image_format = UBWCPLib_RGBA8888,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST = {
	.name = "Shared8 different tile RW linear TP10 Test",
	.func = share8_linear_rw_test,
	.image_format = UBWCPLib_TP10,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST = {
	.name = "Shared8 different tile RW linear P010 Test",
	.func = share8_linear_rw_test,
	.image_format = UBWCPLib_P010,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST = {
	.name = "Shared8 different tile RW linear NV124R Test",
	.func = share8_linear_rw_test,
	.image_format = UBWCPLib_NV124R,
	.image_id = ALL_IDS
};

// Shared image same tile RO Tests
ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RO_ALL_NV12_TEST = {
	.name = "Shared2 same tile RO linear NV12 Test",
	.func = share2_linear_same_tile_ro_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RO_ALL_RGBA8888_TEST = {
	.name = "Shared2 same tile RO linear RGBA8888 Test",
	.func = share2_linear_same_tile_ro_test,
	.image_format = UBWCPLib_RGBA8888,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RO_ALL_TP10_TEST = {
	.name = "Shared2 same tile RO linear TP10 Test",
	.func = share2_linear_same_tile_ro_test,
	.image_format = UBWCPLib_TP10,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RO_ALL_P010_TEST = {
	.name = "Shared2 same tile RO linear P010 Test",
	.func = share2_linear_same_tile_ro_test,
	.image_format = UBWCPLib_P010,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RO_ALL_NV124R_TEST = {
	.name = "Shared2 same tile RO linear NV124R Test",
	.func = share2_linear_same_tile_ro_test,
	.image_format = UBWCPLib_NV124R,
	.image_id = ALL_IDS
};

// Shared image same tile RW Tests
ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RW_ALL_NV12_TEST = {
	.name = "Shared2 same tile RW linear NV12 Test",
	.func = share2_linear_same_tile_rw_test,
	.image_format = UBWCPLib_NV12,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RW_ALL_RGBA8888_TEST = {
	.name = "Shared2 same tile RW linear RGBA8888 Test",
	.func = share2_linear_same_tile_rw_test,
	.image_format = UBWCPLib_RGBA8888,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RW_ALL_TP10_TEST = {
	.name = "Shared2 same tile RW linear TP10 Test",
	.func = share2_linear_same_tile_rw_test,
	.image_format = UBWCPLib_TP10,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RW_ALL_P010_TEST = {
	.name = "Shared2 same tile RW linear P010 Test",
	.func = share2_linear_same_tile_rw_test,
	.image_format = UBWCPLib_P010,
	.image_id = ALL_IDS
};

ubwcp_test_meta SHARED2_SAME_TILE_LINEAR_RW_ALL_NV124R_TEST = {
	.name = "Shared2 same tile RW linear NV124R Test",
	.func = share2_linear_same_tile_rw_test,
	.image_format = UBWCPLib_NV124R,
	.image_id = ALL_IDS
};

//Async tests
ubwcp_test_meta ASYNC_COMPLETE_RW_ID1_NV12_TEST = {
        .name = "Async complete RW NV12 ID1 Test",
        .func = complete_rw_test,
        .image_format = UBWCPLib_NV12,
        .image_id = 1,
        .wait_for_input = true
};

ubwcp_test_meta ASYNC_COMPLETE_RO_ID1_NV12_TEST = {
        .name = "Async complete RO NV12 ID1 Test",
        .func = complete_ro_test,
        .image_format = UBWCPLib_NV12,
        .image_id = 1,
        .wait_for_input = true
};

// Adversarial tests

ubwcp_test_meta ADVERSARIAL_STRIDE_ALIGNMENT_TEST = {
	.name = "Adversarial stride alignment Test",
	.func = adversarial_align_test,
	.image_format = UBWCPLib_NV12,
	.image_id = 1,
};

/////////////////////////////////////////////////////////
// Test Plans

// RW Test Plans
ubwcp_test_plan COMPLETE_RW_ALL_NV12_TEST_PLAN = {
	.name = "Complete RW All NV12 Test Plan",
	.test_plan_id = 1,
	.test_list = {COMPLETE_RW_ALL_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ALL_RGBA8888_TEST_PLAN = {
	.name = "Complete RW All RGBA8888 Test Plan",
	.test_plan_id = 2,
	.test_list = {COMPLETE_RW_ALL_RGBA8888_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ALL_NV124R_TEST_PLAN = {
	.name = "Complete RW All NV124R Test Plan",
	.test_plan_id = 3,
	.test_list = {COMPLETE_RW_ALL_NV124R_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ALL_TP10_TEST_PLAN = {
	.name = "Complete RW All TP10 Test Plan",
	.test_plan_id = 4,
	.test_list = {COMPLETE_RW_ALL_TP10_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ALL_P010_TEST_PLAN = {
	.name = "Complete RW All P010 Test Plan",
	.test_plan_id = 5,
	.test_list = {COMPLETE_RW_ALL_P010_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ALL_P016_TEST_PLAN = {
	.name = "Complete RW All P016 Test Plan",
	.test_plan_id = 6,
	.test_list = {COMPLETE_RW_ALL_P016_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ID1_TEST_PLAN = {
	.name = "Complete RW NV12 ID1 Test Plan",
	.test_plan_id = 7,
	.test_list = {COMPLETE_RW_ID1_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ALL_IMAGES_TEST_PLAN = {
	.name = "Complete RW All Images Test Plan",
	.test_plan_id = 8,
	.test_list = {COMPLETE_RW_ALL_IMAGE_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ALL_IMAGES_TEST_PLAN_STRESS = {
	.name = "Complete RW All Images Test Plan Stress",
	.test_plan_id = 9,
	.test_list = {COMPLETE_RW_ALL_IMAGE_TEST},
	.num_tests = 1,
	.num_test_iters = 100,
	.share_image = false
};

// RO Test plans
ubwcp_test_plan COMPLETE_RO_ALL_NV12_TEST_PLAN = {
	.name = "Complete RO All NV12 Test Plan",
	.test_plan_id = 10,
	.test_list = {COMPLETE_RO_ALL_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RO_ALL_RGBA8888_TEST_PLAN = {
	.name = "Complete RO All RGBA8888 Test Plan",
	.test_plan_id = 11,
	.test_list = {COMPLETE_RO_ALL_RGBA8888_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RO_ALL_TP10_TEST_PLAN = {
	.name = "Complete RO All TP10 Test Plan",
	.test_plan_id = 12,
	.test_list = {COMPLETE_RO_ALL_TP10_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RO_ALL_P010_TEST_PLAN = {
	.name = "Complete RO All P010 Test Plan",
	.test_plan_id = 13,
	.test_list = {COMPLETE_RO_ALL_P010_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

// Multi threaded RW with different images
ubwcp_test_plan MULTI_4_COMPLETE_RW_ALL_NV12_TEST_PLAN = {
	.name = "Four thread complete RW All NV12 Test Plan",
	.test_plan_id = 14,
	.test_list = {COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RW_ALL_NV12_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RW_ALL_RGBA8888_TEST_PLAN = {
	.name = "Four thread complete RW All RGBA8888 Test Plan",
	.test_plan_id = 15,
	.test_list = {COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RW_ALL_RGBA8888_TEST},
	.num_tests = 4,
	.num_test_iters = 10,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RW_ALL_TP10_TEST_PLAN = {
	.name = "Four thread complete RW All TP10 Test Plan",
	.test_plan_id = 16,
	.test_list = {COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RW_ALL_TP10_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RW_ALL_P010_TEST_PLAN = {
	.name = "Four thread complete RW All P010 Test Plan",
	.test_plan_id = 17,
	.test_list = {COMPLETE_RW_ALL_P010_TEST, COMPLETE_RW_ALL_P010_TEST, COMPLETE_RW_ALL_P010_TEST, COMPLETE_RW_ALL_P010_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RW_RO_ALL_NV12_TEST_PLAN = {
	.name = "Four thread complete RW and RO All NV12 Test Plan",
	.test_plan_id = 18,
	.test_list = {COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RO_ALL_NV12_TEST, COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RO_ALL_NV12_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RW_RO_ALL_RGBA8888_TEST_PLAN = {
	.name = "Four thread complete RW and RO All RGBA8888 Test Plan",
	.test_plan_id = 19,
	.test_list = {COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_RGBA8888_TEST, COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_RGBA8888_TEST},
	.num_tests = 4,
	.num_test_iters = 10,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RW_RO_ALL_TP10_TEST_PLAN = {
	.name = "Four thread complete RW and RO All TP10 Test Plan",
	.test_plan_id = 20,
	.test_list = {COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RO_ALL_TP10_TEST, COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RO_ALL_TP10_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RW_RO_ALL_P010_TEST_PLAN = {
	.name = "Four thread complete RW and RO All P010 Test Plan",
	.test_plan_id = 21,
	.test_list = {COMPLETE_RW_ALL_P010_TEST, COMPLETE_RO_ALL_P010_TEST, COMPLETE_RW_ALL_P010_TEST, COMPLETE_RO_ALL_P010_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};
ubwcp_test_plan MULTI_COMPLETE_RW_ALL_NV12_RGBA8888_TP10_P010_TEST_PLAN = {
	.name = "Multi-thread complete RW All NV12 RGBA8888 TP10 P010 Test Plan",
	.test_plan_id = 22,
	.test_list = {COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RW_ALL_P010_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

// Multi threaded RO with different images
ubwcp_test_plan MULTI_4_COMPLETE_RO_ALL_NV12_TEST_PLAN = {
	.name = "Four thread complete RO All NV12 Test Plan",
	.test_plan_id = 23,
	.test_list = {COMPLETE_RO_ALL_NV12_TEST, COMPLETE_RO_ALL_NV12_TEST, COMPLETE_RO_ALL_NV12_TEST, COMPLETE_RO_ALL_NV12_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RO_ALL_RGBA8888_TEST_PLAN = {
	.name = "Four thread complete RO All RGBA8888 Test Plan",
	.test_plan_id = 24,
	.test_list = {COMPLETE_RO_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_RGBA8888_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RO_ALL_TP10_TEST_PLAN = {
	.name = "Four thread complete RO All TP10 Test Plan",
	.test_plan_id = 25,
	.test_list = {COMPLETE_RO_ALL_TP10_TEST, COMPLETE_RO_ALL_TP10_TEST, COMPLETE_RO_ALL_TP10_TEST, COMPLETE_RO_ALL_TP10_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RO_ALL_P010_TEST_PLAN = {
	.name = "Four thread complete RO All P010 Test Plan",
	.test_plan_id = 26,
	.test_list = {COMPLETE_RO_ALL_P010_TEST, COMPLETE_RO_ALL_P010_TEST, COMPLETE_RO_ALL_P010_TEST, COMPLETE_RO_ALL_P010_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan MULTI_COMPLETE_RO_ALL_NV12_RGBA8888_TP10_P010_TEST_PLAN = {
	.name = "Multi-thread complete RO All NV12 RGBA8888 TP10 P010 Test Plan",
	.test_plan_id = 27,
	.test_list = {COMPLETE_RO_ALL_NV12_TEST, COMPLETE_RO_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_TP10_TEST, COMPLETE_RO_ALL_P010_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = false
};

// Multi threaded shared image RO with different tiles
ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_NV12_TEST_PLAN = {
	.name = "Four thread shared image diff tile linear RO All NV12 Test Plan",
	.test_plan_id = 28,
	.test_list = {SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV12_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV12_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV12_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV12_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = true,
	.pre_init_image = true
};

ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_RGBA8888_TEST_PLAN = {
	.name = "Four thread shared image diff tile linear RO All RGBA8888 Test Plan",
	.test_plan_id = 29,
	.test_list = {SHARED4_DIFF_TILE_LINEAR_RO_ALL_RGBA8888_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_RGBA8888_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_RGBA8888_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_RGBA8888_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = true,
	.pre_init_image = true
};

ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_TP10_TEST_PLAN = {
	.name = "Four thread shared image diff tile linear RO All TP10 Test Plan",
	.test_plan_id = 30,
	.test_list = {SHARED4_DIFF_TILE_LINEAR_RO_ALL_TP10_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_TP10_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_TP10_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_TP10_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = true,
	.pre_init_image = true
};

ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_P010_TEST_PLAN = {
	.name = "Four thread shared image diff tile linear RO All P010 Test Plan",
	.test_plan_id = 31,
	.test_list = {SHARED4_DIFF_TILE_LINEAR_RO_ALL_P010_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_P010_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_P010_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_P010_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = true,
	.pre_init_image = true
};

// Multi threaded shared image RW with different tiles
ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_NV12_TEST_PLAN = {
	.name = "Four thread shared image diff tile linear RW All NV12 Test Plan",
	.test_plan_id = 32,
	.test_list = {SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_RGBA8888_TEST_PLAN = {
	.name = "Four thread shared image diff tile linear RW All RGBA8888 Test Plan",
	.test_plan_id = 33,
	.test_list = {SHARED4_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_TP10_TEST_PLAN = {
	.name = "Four thread shared image diff tile linear RW All TP10 Test Plan",
	.test_plan_id = 34,
	.test_list = {SHARED4_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_P010_TEST_PLAN = {
	.name = "Four thread shared image diff tile linear RW All P010 Test Plan",
	.test_plan_id = 35,
	.test_list = {SHARED4_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_P010_TEST},
	.num_tests = 4,
	.num_test_iters = 1,
	.share_image = true
};

// Multi threaded shared image RO with same tile
ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_NV12_TEST_PLAN = {
	.name = "Two thread shared image same tile lnear RO All NV12 Test Plan",
	.test_plan_id = 36,
	.test_list = {SHARED2_SAME_TILE_LINEAR_RO_ALL_NV12_TEST, SHARED2_SAME_TILE_LINEAR_RO_ALL_NV12_TEST},
	.num_tests = 2,
	.num_test_iters = 1,
	.share_image = true,
	.pre_init_image = true
};

ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_RGBA8888_TEST_PLAN = {
	.name = "Two thread shared image same tile linear RO All RGBA8888 Test Plan",
	.test_plan_id = 37,
	.test_list = {SHARED2_SAME_TILE_LINEAR_RO_ALL_RGBA8888_TEST, SHARED2_SAME_TILE_LINEAR_RO_ALL_RGBA8888_TEST},
	.num_tests = 2,
	.num_test_iters = 1,
	.share_image = true,
	.pre_init_image = true
};

ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_TP10_TEST_PLAN = {
	.name = "Two thread shared image same tile linear RO All TP10 Test Plan",
	.test_plan_id = 38,
	.test_list = {SHARED2_SAME_TILE_LINEAR_RO_ALL_TP10_TEST, SHARED2_SAME_TILE_LINEAR_RO_ALL_TP10_TEST},
	.num_tests = 2,
	.num_test_iters = 1,
	.share_image = true,
	.pre_init_image = true
};

ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_P010_TEST_PLAN = {
	.name = "Two thread shared image same tile linear RO All P010 Test Plan",
	.test_plan_id = 39,
	.test_list = {SHARED2_SAME_TILE_LINEAR_RO_ALL_P010_TEST, SHARED2_SAME_TILE_LINEAR_RO_ALL_P010_TEST},
	.num_tests = 2,
	.num_test_iters = 1,
	.share_image = true,
	.pre_init_image = true
};

// Multi threaded shared image RW with same tile
ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_NV12_TEST_PLAN = {
	.name = "Two thread shared image same tile linear RW All NV12 Test Plan",
	.test_plan_id = 40,
	.test_list = {SHARED2_SAME_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED2_SAME_TILE_LINEAR_RW_ALL_NV12_TEST},
	.num_tests = 2,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_RGBA8888_TEST_PLAN = {
	.name = "Two thread shared image same tile linear RW All RGBA8888 Test Plan",
	.test_plan_id = 41,
	.test_list = {SHARED2_SAME_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED2_SAME_TILE_LINEAR_RW_ALL_RGBA8888_TEST},
	.num_tests = 2,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_TP10_TEST_PLAN = {
	.name = "Two thread shared image same tile linear RW All TP10 Test Plan",
	.test_plan_id = 42,
	.test_list = {SHARED2_SAME_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED2_SAME_TILE_LINEAR_RW_ALL_TP10_TEST},
	.num_tests = 2,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_P010_TEST_PLAN = {
	.name = "Two thread shared image same tile linear RW All P010 Test Plan",
	.test_plan_id = 43,
	.test_list = {SHARED2_SAME_TILE_LINEAR_RW_ALL_P010_TEST, SHARED2_SAME_TILE_LINEAR_RW_ALL_P010_TEST},
	.num_tests = 2,
	.num_test_iters = 1,
	.share_image = true
};

// Async test
ubwcp_test_plan ASYNC_COMPLETE_RW_ID1_NV12_TEST_PLAN = {
        .name = "Async complete RW ID1 NV12 Test Plan",
        .test_plan_id = 44,
        .test_list = {ASYNC_COMPLETE_RW_ID1_NV12_TEST},
        .num_tests = 1,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan ASYNC_COMPLETE_RO_ID1_NV12_TEST_PLAN = {
        .name = "Async complete RO ID1 NV12 Test Plan",
        .test_plan_id = 45,
        .test_list = {ASYNC_COMPLETE_RO_ID1_NV12_TEST},
        .num_tests = 1,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan COMPLETE_RO_ALL_LINEAR_TEST_PLAN = {
	.name = "Complete RO All LINEAR Test Plan",
	.test_plan_id = 46,
	.test_list = {COMPLETE_RO_ALL_LINEAR_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ALL_LINEAR_TEST_PLAN = {
	.name = "Complete RW All LINEAR Test Plan",
	.test_plan_id = 47,
	.test_list = {COMPLETE_RW_ALL_LINEAR_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RO_ID9_PERF_NV12_TEST_PLAN = {
	.name = "Complete RO NV12 Perf Test Plan",
	.test_plan_id = 48,
	.test_list = {COMPLETE_RO_ID9_PERF_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.num_test_plan_iters = 10,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RO_ID9_PERF_LINEAR_TEST_PLAN = {
	.name = "Complete RO LINEAR Perf Test Plan",
	.test_plan_id = 49,
	.test_list = {COMPLETE_RO_ID9_PERF_LINEAR_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.num_test_plan_iters = 10,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ID9_PERF_NV12_TEST_PLAN = {
	.name = "Complete RW NV12 Perf Test Plan",
	.test_plan_id = 50,
	.test_list = {COMPLETE_RW_ID9_PERF_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.num_test_plan_iters = 10,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RW_ID9_PERF_LINEAR_TEST_PLAN = {
	.name = "Complete RW LINEAR Perf Test Plan",
	.test_plan_id = 51,
	.test_list = {COMPLETE_RW_ID9_PERF_LINEAR_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.num_test_plan_iters = 10,
	.share_image = false
};

ubwcp_test_plan COMPLETE_RO_2GB_ULAPA_RANGE_NV12_TEST_PLAN = {
	.name = "Complete RO 2GB ULA PA Range NV12 Test Plan",
	.test_plan_id = 52,
	.test_list = {COMPLETE_RO_2GB_ULAPA_RANGE_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false,
};

ubwcp_test_plan COMPLETE_RO_227_DESC_NV12_TEST_PLAN = {
	.name = "Complete RO 227 DESCRIPTOR NV12 Test Plan",
	.test_plan_id = 53,
	.test_list = {COMPLETE_RO_227_DESC_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false,
};

ubwcp_test_plan COMPLETE_RW_2GB_ULAPA_RANGE_NV12_TEST_PLAN = {
	.name = "Complete RW 2GB ULA PA Range NV12 Test Plan",
	.test_plan_id = 54,
	.test_list = {COMPLETE_RW_2GB_ULAPA_RANGE_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false,
};

ubwcp_test_plan COMPLETE_RW_227_DESC_NV12_TEST_PLAN = {
	.name = "Complete RW 227 DESCRIPTOR NV12 Test Plan",
	.test_plan_id = 55,
	.test_list = {COMPLETE_RW_227_DESC_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false,
};

ubwcp_test_plan COMPLETE_RO_ALL_NV124R_TEST_PLAN = {
        .name = "Complete RO All NV124R Test Plan",
        .test_plan_id = 56,
        .test_list = {COMPLETE_RO_ALL_NV124R_TEST},
        .num_tests = 1,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan COMPLETE_RO_ALL_P016_TEST_PLAN = {
        .name = "Complete RO All P016 Test Plan",
        .test_plan_id = 57,
        .test_list = {COMPLETE_RO_ALL_P016_TEST},
        .num_tests = 1,
        .num_test_iters = 1,
        .share_image = false
};

// Addtional NV124R tests
ubwcp_test_plan MULTI_4_COMPLETE_RW_ALL_NV124R_TEST_PLAN = {
        .name = "Four thread complete RW All NV124R Test Plan",
        .test_plan_id = 58,
        .test_list = {COMPLETE_RW_ALL_NV124R_TEST, COMPLETE_RW_ALL_NV124R_TEST, COMPLETE_RW_ALL_NV124R_TEST, COMPLETE_RW_ALL_NV124R_TEST},
        .num_tests = 4,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RW_RO_ALL_NV124R_TEST_PLAN = {
        .name = "Four thread complete RW and RO All NV124R Test Plan",
        .test_plan_id = 59,
        .test_list = {COMPLETE_RW_ALL_NV124R_TEST, COMPLETE_RO_ALL_NV124R_TEST, COMPLETE_RW_ALL_NV124R_TEST, COMPLETE_RO_ALL_NV124R_TEST},
        .num_tests = 4,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan MULTI_COMPLETE_RW_ALL_NV12_RGBA8888_TP10_P010_NV124R_TEST_PLAN = {
        .name = "Multi-thread complete RW All NV12 RGBA8888 TP10 P010 NV124R Test Plan",
        .test_plan_id = 60,
        .test_list = {COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RW_ALL_P010_TEST, COMPLETE_RW_ALL_NV124R_TEST},
        .num_tests = 5,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan MULTI_4_COMPLETE_RO_ALL_NV124R_TEST_PLAN = {
        .name = "Four thread complete RO All NV124R Test Plan",
        .test_plan_id = 61,
        .test_list = {COMPLETE_RO_ALL_NV124R_TEST, COMPLETE_RO_ALL_NV124R_TEST, COMPLETE_RO_ALL_NV124R_TEST, COMPLETE_RO_ALL_NV124R_TEST},
        .num_tests = 4,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan MULTI_COMPLETE_RO_ALL_NV12_RGBA8888_TP10_P010_NV124R_TEST_PLAN = {
        .name = "Multi-thread complete RO All NV12 RGBA8888 TP10 P010 NV124R Test Plan",
        .test_plan_id = 62,
        .test_list = {COMPLETE_RO_ALL_NV12_TEST, COMPLETE_RO_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_TP10_TEST, COMPLETE_RO_ALL_P010_TEST, COMPLETE_RO_ALL_NV124R_TEST},
        .num_tests = 5,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_NV124R_TEST_PLAN = {
        .name = "Four thread shared image diff tile linear RO All NV124R Test Plan",
        .test_plan_id = 63,
        .test_list = {SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV124R_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV124R_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV124R_TEST, SHARED4_DIFF_TILE_LINEAR_RO_ALL_NV124R_TEST},
        .num_tests = 4,
        .num_test_iters = 1,
        .share_image = true,
        .pre_init_image = true
};

ubwcp_test_plan MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_NV124R_TEST_PLAN = {
        .name = "Four thread shared image diff tile linear RW All NV124R Test Plan",
        .test_plan_id = 64,
        .test_list = {SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED4_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST},
        .num_tests = 4,
        .num_test_iters = 1,
        .share_image = true
};

ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_NV124R_TEST_PLAN = {
        .name = "Two thread shared image same tile lnear RO All NV124R Test Plan",
        .test_plan_id = 65,
        .test_list = {SHARED2_SAME_TILE_LINEAR_RO_ALL_NV124R_TEST, SHARED2_SAME_TILE_LINEAR_RO_ALL_NV124R_TEST},
        .num_tests = 2,
        .num_test_iters = 1,
        .share_image = true,
        .pre_init_image = true
};

ubwcp_test_plan MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_NV124R_TEST_PLAN = {
        .name = "Two thread shared image same tile linear RW All NV124R Test Plan",
        .test_plan_id = 66,
        .test_list = {SHARED2_SAME_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED2_SAME_TILE_LINEAR_RW_ALL_NV124R_TEST},
        .num_tests = 2,
        .num_test_iters = 1,
        .share_image = true
};

ubwcp_test_plan MULTI_10_COMPLETE_RW_ALL_NV12_RGBA8888_TP10_P010_NV124R_TEST_PLAN = {
        .name = "10 thread complete RW All NV12 RGBA8888 TP10 P010 NV124R Test Plan",
        .test_plan_id = 67,
        .test_list = {COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RW_ALL_P010_TEST, COMPLETE_RW_ALL_NV124R_TEST,\
		     COMPLETE_RW_ALL_NV12_TEST, COMPLETE_RW_ALL_RGBA8888_TEST, COMPLETE_RW_ALL_TP10_TEST, COMPLETE_RW_ALL_P010_TEST, COMPLETE_RW_ALL_NV124R_TEST},
         .num_tests = 10,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan MULTI_10_COMPLETE_RO_ALL_NV12_RGBA8888_TP10_P010_NV124R_TEST_PLAN = {
        .name = "10 thread complete RO All NV12 RGBA8888 TP10 P010 NV124R Test Plan",
        .test_plan_id = 68,
        .test_list = {COMPLETE_RO_ALL_NV12_TEST, COMPLETE_RO_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_TP10_TEST, COMPLETE_RO_ALL_P010_TEST, COMPLETE_RO_ALL_NV124R_TEST,\
		     COMPLETE_RO_ALL_NV12_TEST, COMPLETE_RO_ALL_RGBA8888_TEST, COMPLETE_RO_ALL_TP10_TEST, COMPLETE_RO_ALL_P010_TEST, COMPLETE_RO_ALL_NV124R_TEST},
        .num_tests = 10,
        .num_test_iters = 1,
        .share_image = false
};

ubwcp_test_plan CUSTOM_COMPLETE_RW_TEST_PLAN = {
	.name = "Custom complete RW Test Plan",
	.test_plan_id = 69,
	.test_list = {CUSTOM_COMPLETE_RW_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan CUSTOM_COMPLETE_RO_TEST_PLAN = {
	.name = "Custom complete RO Test Plan",
	.test_plan_id = 70,
	.test_list = {CUSTOM_COMPLETE_RO_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan WHOLE_RW_ALL_NV12_TEST_PLAN = {
	.name = "Whole RW All NV12 Test Plan",
	.test_plan_id = 71,
	.test_list = {WHOLE_RW_ALL_NV12_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

ubwcp_test_plan ADVERSARIAL_STRIDE_ALIGNMENT_TEST_PLAN = {
	.name = "Adversarial stride alignment Test Plan",
	.test_plan_id = 72,
	.test_list = {ADVERSARIAL_STRIDE_ALIGNMENT_TEST},
	.num_tests = 1,
	.num_test_iters = 1,
	.share_image = false
};

// 8 threaded shared image RW with different tiles
ubwcp_test_plan MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_NV12_TEST_PLAN = {
	.name = "Eight thread shared image diff tile linear RW All NV12 Test Plan",
	.test_plan_id = 73,
	.test_list = {SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST,
		      SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV12_TEST},
	.num_tests = 8,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_RGBA8888_TEST_PLAN = {
	.name = "Eight thread shared image diff tile linear RW All RGBA8888 Test Plan",
	.test_plan_id = 74,
	.test_list = {SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST,
		      SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_RGBA8888_TEST},
	.num_tests = 8,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_TP10_TEST_PLAN = {
	.name = "Eight thread shared image diff tile linear RW All TP10 Test Plan",
	.test_plan_id = 75,
	.test_list = {SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST,
		      SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_TP10_TEST},
	.num_tests = 8,
	.num_test_iters = 1,
	.share_image = true
};

ubwcp_test_plan MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_P010_TEST_PLAN = {
	.name = "Eight thread shared image diff tile linear RW All P010 Test Plan",
	.test_plan_id = 76,
	.test_list = {SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST,
		      SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_P010_TEST},
	.num_tests = 8,
	.num_test_iters = 1,
	.share_image = true
};
ubwcp_test_plan MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_NV124R_TEST_PLAN = {
        .name = "Eight thread shared image diff tile linear RW All NV124R Test Plan",
        .test_plan_id = 77,
        .test_list = {SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST,
		      SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST, SHARED8_DIFF_TILE_LINEAR_RW_ALL_NV124R_TEST},
        .num_tests = 8,
        .num_test_iters = 1,
        .share_image = true
};

ubwcp_test_plan MULTI_8_COMPLETE_RW_VERT_COL4B_ALL_NV12_TP10_TEST_PLAN = {
        .name = "Eight thread complete RW vert col 4B All NV12 and TP10 Test Plan",
        .test_plan_id = 78,
        .test_list = {COMPLETE_RW_ALL_VERT_COL4B_NV12_TEST, COMPLETE_RW_ALL_VERT_COL4B_TP10_TEST, COMPLETE_RW_ALL_VERT_COL4B_NV12_TEST, COMPLETE_RW_ALL_VERT_COL4B_TP10_TEST,
		      COMPLETE_RW_ALL_VERT_COL4B_NV12_TEST, COMPLETE_RW_ALL_VERT_COL4B_TP10_TEST, COMPLETE_RW_ALL_VERT_COL4B_NV12_TEST, COMPLETE_RW_ALL_VERT_COL4B_TP10_TEST},
        .num_tests = 8,
        .num_test_iters = 1,
        .share_image = false
};

// Test plan list
ubwcp_test_plan all_test_plans[] {
	COMPLETE_RW_ALL_NV12_TEST_PLAN,
	COMPLETE_RW_ALL_RGBA8888_TEST_PLAN,
	COMPLETE_RW_ALL_NV124R_TEST_PLAN,
	COMPLETE_RW_ALL_TP10_TEST_PLAN,
	COMPLETE_RW_ALL_P010_TEST_PLAN,
	COMPLETE_RW_ALL_P016_TEST_PLAN,
	COMPLETE_RW_ID1_TEST_PLAN,
	COMPLETE_RW_ALL_IMAGES_TEST_PLAN,
	COMPLETE_RW_ALL_IMAGES_TEST_PLAN_STRESS,
	COMPLETE_RO_ALL_NV12_TEST_PLAN,
	COMPLETE_RO_ALL_RGBA8888_TEST_PLAN,
	COMPLETE_RO_ALL_TP10_TEST_PLAN,
	COMPLETE_RO_ALL_P010_TEST_PLAN,
	MULTI_4_COMPLETE_RW_ALL_NV12_TEST_PLAN,
	MULTI_4_COMPLETE_RW_ALL_RGBA8888_TEST_PLAN,
	MULTI_4_COMPLETE_RW_ALL_TP10_TEST_PLAN,
	MULTI_4_COMPLETE_RW_ALL_P010_TEST_PLAN,
	MULTI_4_COMPLETE_RW_RO_ALL_NV12_TEST_PLAN,
	MULTI_4_COMPLETE_RW_RO_ALL_RGBA8888_TEST_PLAN,
	MULTI_4_COMPLETE_RW_RO_ALL_TP10_TEST_PLAN,
	MULTI_4_COMPLETE_RW_RO_ALL_P010_TEST_PLAN,
	MULTI_COMPLETE_RW_ALL_NV12_RGBA8888_TP10_P010_TEST_PLAN,
	MULTI_4_COMPLETE_RO_ALL_NV12_TEST_PLAN,
	MULTI_4_COMPLETE_RO_ALL_RGBA8888_TEST_PLAN,
	MULTI_4_COMPLETE_RO_ALL_TP10_TEST_PLAN,
	MULTI_4_COMPLETE_RO_ALL_P010_TEST_PLAN,
	MULTI_COMPLETE_RO_ALL_NV12_RGBA8888_TP10_P010_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_NV12_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_RGBA8888_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_TP10_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_P010_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_NV12_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_RGBA8888_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_TP10_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_P010_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_NV12_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_RGBA8888_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_TP10_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_P010_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_NV12_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_RGBA8888_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_TP10_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_P010_TEST_PLAN,
	ASYNC_COMPLETE_RW_ID1_NV12_TEST_PLAN,
	ASYNC_COMPLETE_RO_ID1_NV12_TEST_PLAN,
	COMPLETE_RO_ALL_LINEAR_TEST_PLAN,
	COMPLETE_RW_ALL_LINEAR_TEST_PLAN,
	COMPLETE_RO_ID9_PERF_NV12_TEST_PLAN,
	COMPLETE_RO_ID9_PERF_LINEAR_TEST_PLAN,
	COMPLETE_RW_ID9_PERF_NV12_TEST_PLAN,
	COMPLETE_RW_ID9_PERF_LINEAR_TEST_PLAN,
	COMPLETE_RO_2GB_ULAPA_RANGE_NV12_TEST_PLAN,
	COMPLETE_RO_227_DESC_NV12_TEST_PLAN,
	COMPLETE_RW_2GB_ULAPA_RANGE_NV12_TEST_PLAN,
	COMPLETE_RW_227_DESC_NV12_TEST_PLAN,
	COMPLETE_RO_ALL_NV124R_TEST_PLAN,
	COMPLETE_RO_ALL_P016_TEST_PLAN,
	MULTI_4_COMPLETE_RW_ALL_NV124R_TEST_PLAN,
	MULTI_4_COMPLETE_RW_RO_ALL_NV124R_TEST_PLAN,
	MULTI_COMPLETE_RW_ALL_NV12_RGBA8888_TP10_P010_NV124R_TEST_PLAN,
	MULTI_4_COMPLETE_RO_ALL_NV124R_TEST_PLAN,
	MULTI_COMPLETE_RO_ALL_NV12_RGBA8888_TP10_P010_NV124R_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RO_ALL_NV124R_TEST_PLAN,
	MULTI_SHARED4_DIFF_TILE_LIN_RW_ALL_NV124R_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RO_ALL_NV124R_TEST_PLAN,
	MULTI_SHARED2_SAME_TILE_LIN_RW_ALL_NV124R_TEST_PLAN,
	MULTI_10_COMPLETE_RW_ALL_NV12_RGBA8888_TP10_P010_NV124R_TEST_PLAN,
	MULTI_10_COMPLETE_RO_ALL_NV12_RGBA8888_TP10_P010_NV124R_TEST_PLAN,
	CUSTOM_COMPLETE_RW_TEST_PLAN,
	CUSTOM_COMPLETE_RO_TEST_PLAN,
	WHOLE_RW_ALL_NV12_TEST_PLAN,
	ADVERSARIAL_STRIDE_ALIGNMENT_TEST_PLAN,
	MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_NV12_TEST_PLAN,
	MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_RGBA8888_TEST_PLAN,
	MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_TP10_TEST_PLAN,
	MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_P010_TEST_PLAN,
	MULTI_SHARED8_DIFF_TILE_LIN_RW_ALL_NV124R_TEST_PLAN,
	MULTI_8_COMPLETE_RW_VERT_COL4B_ALL_NV12_TP10_TEST_PLAN
};

/////////////////////////////////////////////////////////
// Client argument parsing helper functions

#define OPT_H_STR "h"
#define OPT_L_STR "l"
#define OPT_D_STR "d"
#define OPT_IMAGE_PATH_STR "image_path"
#define OPT_TP_ID_STR "tp_id"
#define OPT_RW_TEST_STR "rw_test"
#define OPT_RO_TEST_STR "ro_test"
#define OPT_DUMP_FILE_STR "dump_file"
#define OPT_PAUSE_ON_FAILURE_STR "pause_on_failure"
#define OPT_NUM_ITERS_STR "num_iters"
#define OPT_NUM_THREADS_STR "num_threads"
#define OPT_ACCESS_TYPE_STR "access_type"
#define OPT_IMAGE_FORMAT_STR "image_format"
#define OPT_IMAGE_ID_STR "image_id"
#define OPT_NUM_TP_ITERS_STR "num_tp_iters"

enum user_option
{
	OPT_H = 0,
	OPT_L,
	OPT_D,
	OPT_IMAGE_PATH,
	OPT_TP_ID,
	OPT_RW_TEST,
	OPT_RO_TEST,
	OPT_DUMP_FILE,
	OPT_PAUSE_ON_FAILURE,
	OPT_NUM_ITERS,
	OPT_NUM_THREADS,
	OPT_ACCESS_TYPE,
	OPT_IMAGE_FORMAT,
	OPT_IMAGE_ID,
	OPT_NUM_TP_ITERS,
	OPT_LAST
};

int get_option(const char *str, enum user_option *op)
{
	int ret = 0;
	str++;
	if (strcmp(str, OPT_H_STR) == 0) {
		*op = OPT_H;
	} else if (strcmp(str, OPT_L_STR) == 0) {
		*op = OPT_L;
	} else if (strcmp(str, OPT_D_STR) == 0) {
		*op = OPT_D;
	} else if (strcmp(str, OPT_IMAGE_PATH_STR) == 0) {
		*op = OPT_IMAGE_PATH;
	} else if (strcmp(str, OPT_TP_ID_STR) == 0) {
		*op = OPT_TP_ID;
	} else if (strcmp(str, OPT_RW_TEST_STR) == 0) {
		*op = OPT_RW_TEST;
	} else if (strcmp(str, OPT_RO_TEST_STR) == 0) {
		*op = OPT_RO_TEST;
	} else if (strcmp(str, OPT_DUMP_FILE_STR) == 0) {
		*op = OPT_DUMP_FILE;
	} else if (strcmp(str, OPT_PAUSE_ON_FAILURE_STR) == 0) {
		*op = OPT_PAUSE_ON_FAILURE;
	} else if (strcmp(str, OPT_NUM_ITERS_STR) == 0) {
		*op = OPT_NUM_ITERS;
	} else if (strcmp(str, OPT_NUM_THREADS_STR) == 0) {
		*op = OPT_NUM_THREADS;
	} else if (strcmp(str, OPT_ACCESS_TYPE_STR) == 0) {
		*op = OPT_ACCESS_TYPE;
	} else if (strcmp(str, OPT_IMAGE_FORMAT_STR) == 0) {
		*op = OPT_IMAGE_FORMAT;
	} else if (strcmp(str, OPT_IMAGE_ID_STR) == 0) {
		*op = OPT_IMAGE_ID;
	} else if (strcmp(str, OPT_NUM_TP_ITERS_STR) == 0) {
		*op = OPT_NUM_TP_ITERS;
	} else {

		ret = -EINVAL;
	}

	return ret;
}

static int parse_debug_level(const char *cstr, int *debug_level)
{
	int ret = 0;

	if (strcmp(cstr, "i") == 0) {
		*debug_level = UBWCP_INFO;
	} else if (strcmp(cstr, "w") == 0) {
		*debug_level = UBWCP_WARN;
	} else if (strcmp(cstr, "e") == 0) {
		*debug_level = UBWCP_ERR;
	} else {
		ret = -EINVAL;
	}

	return ret;
}

int parse_test_format_id(const char *cstr, int *test_format, int *test_id)
{
	char *str_base = strdup(cstr);
	char *str = str_base;
	char *token;
	int ret = -EINVAL;
	char *tmp_val;

	if (!str_base)
		return -ENOMEM;

	token = strsep(&str, ",");
	if (token != NULL) {
		*test_format = atoi(token);

		token = strsep(&str, ",");
		if (!token)
			goto err;

		*test_id = atoi(token);
		ret = 0;
	}

err:
	free(str_base);
	return ret;
}

int parse_params(const char *cstr, user_option *op, char **val)
{
	char *str_base = strdup(cstr);
	char *str = str_base;
	char *token;
	int ret = -EINVAL;
	char *tmp_val;
	*val = NULL;

	if (!str_base)
		return -ENOMEM;

	token = strsep(&str, "=");
	if (token != NULL) {
		ret = get_option(token, op);
		if (ret != 0) {
			goto err;
		}

		token = strsep(&str, "=");
		if (!token)
			goto err;

		tmp_val = strdup(token);
		if (!tmp_val)
			goto err;

		*val = tmp_val;
		ret = 0;
	} else {
		ret = get_option(cstr, op);
		*val = NULL;
	}

err:
	free(str_base);
	return ret;
}

static int validate_plan_id(int plan_id)
{
	int ret = -EINVAL;

	int num_plans = sizeof(all_test_plans) / sizeof(ubwcp_test_plan);

	for (int i = 0; i < num_plans; i++) {
		if (all_test_plans[i].test_plan_id == plan_id) {
			ret = 0;
			break;
		}
	}

	return ret;
}

#define CMD_LINE "ubwcp_test -h -l -d=[i|w|e] -dump_file -num_tp_iters=[#iterations] -num_iters=[#iterations] -num_threads=[#threads] -access_type=[access type] -image_format=[image format] -image_id=[image id] -image_path=[Path to test images] -tp_id=[Test plan id] -rw_test=[format number],[image id] -ro_test=[format number],[image id]"
#define CMD_HELP "Description\n\
\t -h\n\
\t\t Print this help message\n\
\n\
\t -l\n\
\t\t Print the list of supported test plans and their test plan ids\n\
\n\
\n\
\t Debug options:\n\
\n\
\t -d=[i|w|e]\n\
\t\t Specify which level of logging are printed for ubwcp_test\n\
\t\t i: Print INFO messages and above\n\
\t\t w: Print WARNING messages and above\n\
\t\t e: Print ERROR messages and above [default]\n\
\n\
\t -dump_file\n\
\t\t When an image comparison in a test fails dump the UBWC-P produced image to the same location as the golden version\n\
\t\t of the image. The name of the dumped file will be the name of the golden image file with '_fail' appended to it\n\
\n\
\n\
\t Override options:\n\
\n\
\t -num_tp_iters=[num iterations]\n\
\t\t Allows the caller to override the number of test plan iterations\n\
\n\
\\t -num_iters=[num iterations]\n\
\t\t Allows the caller to override the number of test iterations within a test plan\n\
\n\
\t -num_threads=[num threads]\n\
\t\t Allows the caller to override the number of threads a test plan has\n\
\t\t The addtional threads will run the same test as the first test in the test plan\n\
\n\
\t -access_type=[access type]\n\
\t\t Allows the caller to override the access type a test plan uses\n\
\t\t Access types:\n\
\t\t    0: Byte linear access to image data planes\n\
\t\t       Will traverse in a linear direction from the start of image.\n\
\t\t       Will access byte by byte.\n\
\t\t       Will access the complete Y and UV data planes of the image, but it won't access\n\
\t\t       any of the padding.\n\
\n\
\t\t    1: Byte linear access to whole image\n\
\t\t       Will traverse in a linear direction from the start of image.\n\
\t\t       Will access byte by byte.\n\
\t\t       Will access the whole image, including any padding.\n\
\n\
\t\t    2: Pixel linear access to image data planes\n\
\t\t       Will traverse in a linear direction from the start of image.\n\
\t\t       Will access pixel by pixel, for each pixel it will read the Y, U and V components\n\
\t\t       of the pixel.\n\
\t\t       Will access the complete Y and UV data planes of the image, but it won't access\n\
\t\t       any of the padding.\n\
\n\
\t\t    3: Pixel vertical access to image data planes\n\
\t\t       Will traverse in a vertical direction from the start of image, will start with\n\
\t\t       the pixels in column 1 (from top to bottom), then will access the pixels in\n\
\t\t       column 2 (from top to bottom), ...\n\
\t\t       Will access pixel by pixel, for each pixel it will read the Y, U and V components\n\
\t\t       of the pixel.\n\
\t\t       Will access the complete Y and UV data planes of the image, but it won't access\n\
\t\t       any of the padding.\n\
\n\
\t\t    4: Will traverse in a vertical direction from the start of image, will start with\n\
\t\t       the bytes in column 1 (from top to bottom), then will access the bytes in\n\
\t\t       column 2 (from top to bottom), ...\n\
\t\t       Each column will be 4 Bytes in width.\n\
\t\t       It won't access any of the padding.\n\
\n\
\t -image_format=[image format]\n\
\t\t Allows the caller to override the image format a test plan uses\n\
\t\t Image formats:\n\
\t\t	-1: All formats\n\
\t\t	0: Linear\n\
\t\t	1: RGBA8888\n\
\t\t	2: NV12\n\
\t\t	5: NV124R\n\
\t\t	8: TP10\n\
\t\t	11: P010\n\
\n\
\t -image_id=[image id]\n\
\t\t Allows the caller to override the test image which is used\n\
\t\t Use -1 to specify all image ids for the given format\
\n\
\n\
\t Test type:\n\
\n\
\t -image_path=[Path to test images]\n\
\t\t Path to the folder which contains the test images\n\
\t\t Required for running UBWC-P Image R/W tests\n\
\n\
\n\
\t Specify Test Plan:\n\
\n\
\t -tp_id=[Test plan id]\n\
\t\t Run the Test Plan with given id\n\
\n\
\t -rw_test=[format number],[image id]\n\
\t\t Run an individual UBWC-P Image R/W test using the set of images in the format folder who have the provided image id\n\
\n\
\t -ro_test=[format number],[image id]\n\
\t\t Run an individual UBWC-P Image RO test using the set of images in the format folder who have the provided image id\n"

static void print_help(void)
{
	UBWCP_LOG(UBWCP_ERR, "%s", CMD_LINE);
	UBWCP_LOG(UBWCP_ERR, "%s", CMD_HELP);
}

static void print_test_plan_list(void)
{
	int num_plans = sizeof(all_test_plans) / sizeof(ubwcp_test_plan);

	UBWCP_LOG(UBWCP_ERR, "Available tests:");
	for (int i = 0; i < num_plans; i++) {
		UBWCP_LOG(UBWCP_ERR, "Test Plan ID:%d Test:%s ", all_test_plans[i].test_plan_id, all_test_plans[i].name);
	}
}

static int get_test_plan(int id, ubwcp_test_plan **test_plan, int num_threads, int image_format, int image_id,
			 struct access_descriptor *access_desc)
{
	int ret = -EINVAL;
	int num_plans = sizeof(all_test_plans) / sizeof(ubwcp_test_plan);

	*test_plan = ( ubwcp_test_plan *)malloc(sizeof(ubwcp_test_plan));
	if (!*test_plan)
		return -ENOMEM;

	for (int i = 0; i < num_plans; i++) {
		if (all_test_plans[i].test_plan_id == id) {
			memcpy(*test_plan, &all_test_plans[i], sizeof(ubwcp_test_plan));

			for (int j = 0; j < (*test_plan)->num_tests; j++) {
				if (image_format != -1)
					(*test_plan)->test_list[j].image_format = image_format;

				if (image_id != -1)
					(*test_plan)->test_list[j].image_id = image_id;

				if (access_desc != NULL)
					(*test_plan)->test_list[j].access_desc = access_desc;
			}

			if (num_threads != -1) {
				for (int i=1; i < num_threads; i++)
					(*test_plan)->test_list[i] = (*test_plan)->test_list[0];

				(*test_plan)->num_tests = num_threads;
			}

			ret = 0;
			break;
		}
	}
	return ret;
}

static int parse_test_plan_id(const char *cstr, int * test_plan_id)
{
	int num = atoi(cstr);
	int ret = validate_plan_id(num);
	if (ret == 0)
		*test_plan_id = num;

	return ret;
}

static int parse_num_iters(const char *cstr, int * num_iters)
{
	int num = atoi(cstr);
	if (num <= 0)
		return -EINVAL;

	*num_iters = num;

	return 0;
}

static int parse_num_threads(const char *cstr, int * num_threads)
{
	int num = atoi(cstr);
	if (num <= 0 || num > MAX_TEST_PLAN_TESTS) {
		UBWCP_LOG(UBWCP_ERR, "Invalid num_threads provided, max:%d", MAX_TEST_PLAN_TESTS);
		return -EINVAL;
	}

	*num_threads = num;

	return 0;
}

static int parse_access_type(const char *cstr, struct access_descriptor **access_desc)
{
	int access_type = atoi(cstr);
	*access_desc = NULL;

	switch(access_type) {
		case BYTE_LINEAR_PLANE_FUNC_TYPE:
			*access_desc = &BYTE_LINEAR_PLANE_ACCESS;
			break;
		case BYTE_LINEAR_WHOLE_FUNC_TYPE:
			*access_desc = &BYTE_LINEAR_WHOLE_ACCESS;
			break;
		case PIXEL_LINEAR_PLANE_FUNC_TYPE:
			*access_desc = &PIXEL_LINEAR_PLANE_ACCESS;
			break;
		case PIXEL_VERTICAL_PLANE_FUNC_TYPE:
			*access_desc = &PIXEL_VERTICAL_PLANE_ACCESS;
			break;
		case BYTE_VERTICAL_COL4B_PLANE_FUNC_TYPE:
			*access_desc = &BYTE_VERTICAL_COL4B_PLANE_ACCESS;
			break;
		default:
			UBWCP_LOG(UBWCP_ERR, "Invalid access_type:%d", access_type);
			return -EINVAL;
	}

	return 0;
}

static int parse_image_format_num(const char *cstr, int *image_format)
{
	int format = atoi(cstr);
	if (format < ALL_UBWCP_FORMATS || format >= UBWCPLib_NUM_FORMATS) {
		UBWCP_LOG(UBWCP_ERR, "Invalid image format, max:%d", UBWCPLib_NUM_FORMATS);
		return -EINVAL;
	}

	*image_format = format;

	return 0;
}

static int parse_image_id(const char *cstr, int *image_id)
{
	int id = atoi(cstr);
	if (id < ALL_IDS) {
		UBWCP_LOG(UBWCP_ERR, "Invalid image id%d", id);
		return -EINVAL;
	}
	*image_id = id;

	return 0;
}

// debug function
static void dump_file_list(void)
{
	for (int i=0; i < UBWCPLib_NUM_FORMATS; i++) {
		UBWCP_LOG(UBWCP_ERR, "\nFor image format format:%d", i);
		for (int j=0; j < test_image_list.format_list[i].num_images; j++) {
			Image_Attrs * at = &test_image_list.format_list[i].image_list[j];
			UBWCP_LOG(UBWCP_ERR, "Image idx:%d id:%d, w:%d, h:%d, str:%d, scan:%d, planpad:%d, linear:%d, format:%d, sz:%d, path:%s",
			     j, at->id, at->w, at->h, at->stride, at->scanlines, at->planar_padding, at->linear, (int)at->format, at->file_size, at->file_path );
		}
	}
}


//////////////////////////////////////////////////////////

int main( int argc, char *argv[] )  {
	int ret = -EINVAL;
	int test_err;
	int debug_level = UBWCP_ERR;
	int test_plan_id = -1, image_format = -1, image_id = -1;
	char *image_path = NULL;
	bool run_tp = false;
	bool run_rw_test = false;
	bool run_ro_test = false;
	int num_iters = -1;
	int num_tp_iters = -1;
	int num_threads = -1;
	struct access_descriptor *access_desc = NULL;

	allocator = new BufferAllocator();
	if (!allocator) {
		UBWCP_LOG(UBWCP_ERR, "No memory");
		return 0;
	}

	//TODO: cleanup test_image_list memory
	for (int i = 1; i < argc; i++) {
		enum user_option op;
		char * value = NULL;
		char * param = argv[i];
		ret = parse_params(param, &op, &value);
		if (ret != 0) {
			UBWCP_LOG(UBWCP_WARN, "Failed to parse param:%s", param );
			break;
		}

		switch (op) {
		case OPT_H:
			print_help();
			goto end;
		case OPT_L:
			print_test_plan_list();
			goto end;
		case OPT_D:
			if (!value)
				goto err;
			ret = parse_debug_level(value, &debug_level);
			free(value);
			if (ret != 0)
				goto err;
			set_debug_level(debug_level);
			UBWCP_LOG(UBWCP_INFO, "Parsed debuglevel:%d", debug_level );
			break;
		case OPT_IMAGE_PATH:
			if (!value)
				goto err;
			image_path = value;
			break;
		case OPT_TP_ID:
			if (!value)
				goto err;
			ret = parse_test_plan_id(value, &test_plan_id);
			free(value);
			if (ret != 0)
				goto err;
			run_tp = true;
			break;
		case OPT_RW_TEST:
			if (!value)
				goto err;
			ret = parse_test_format_id(value, &image_format, &image_id);
			free(value);
			if (ret != 0)
				goto err;
			run_rw_test = true;
			break;
		case OPT_RO_TEST:
			if (!value)
				goto err;
			ret = parse_test_format_id(value, &image_format, &image_id);
			free(value);
			if (ret != 0)
				goto err;
			run_ro_test = true;
			break;
		case OPT_DUMP_FILE:
			dump_diff_to_file = true;
			break;
		case OPT_PAUSE_ON_FAILURE:
			pause_on_failure = true;
			UBWCP_LOG(UBWCP_INFO, "pause_on_failure is enabled");
			break;
		case OPT_NUM_ITERS:
			if (!value)
				goto err;
			ret = parse_num_iters(value, &num_iters);
			free(value);
			if (ret != 0)
				goto err;
			break;
		case OPT_NUM_THREADS:
			if (!value)
				goto err;
			ret = parse_num_threads(value, &num_threads);
			free(value);
			if (ret != 0)
				goto err;
			break;
		case OPT_ACCESS_TYPE:
			if (!value)
				goto err;
			ret = parse_access_type(value, &access_desc);
			free(value);
			if (ret != 0)
				goto err;
			break;
		case OPT_IMAGE_FORMAT:
			if (!value)
				goto err;
			ret = parse_image_format_num(value, &image_format);
			free(value);
			if (ret != 0)
				goto err;
			break;
		case OPT_IMAGE_ID:
			if (!value)
				goto err;
			ret = parse_image_id(value, &image_id);
			free(value);
			if (ret != 0)
				goto err;
			break;
		case OPT_NUM_TP_ITERS:
			if (!value)
				goto err;
			ret = parse_num_iters(value, &num_tp_iters);
			free(value);
			if (ret != 0)
				goto err;
			break;
		default:
			free(value);
			ret = -EINVAL;
			goto err;
		}
	}

	if (image_path) {
		UBWCP_LOG(UBWCP_INFO, "Image path:%s\n", image_path);

		ret = parse_available_images(image_path, &test_image_list);
		if (ret) {
			UBWCP_LOG(UBWCP_ERR, "Failed to parse images err:%d, path:%s", ret, image_path);
			goto end;
		}
	} else {
		UBWCP_LOG(UBWCP_ERR, "Currently all tests require a path to the images folder");
		goto err;
	}

	if (run_rw_test) {
		UBWCP_LOG(UBWCP_INFO, "run custom R/W test with format:%d image id:%d path:%s", image_format, image_id, image_path);

		run_tp = true;
		test_plan_id = 69;
	}

	if (run_ro_test) {
		UBWCP_LOG(UBWCP_INFO, "run custom RO test with format:%d image id:%d path:%s", image_format, image_id, image_path);
		run_tp = true;
		test_plan_id = 70;
	}

	if (run_tp) {
		ubwcp_test_plan *test_plan;

		UBWCP_LOG(UBWCP_INFO, "Run test plan tpid:%d, path:%s", test_plan_id, image_path);

		ret = get_test_plan(test_plan_id, &test_plan, num_threads, image_format, image_id, access_desc);
		if (ret != 0) {
			UBWCP_LOG(UBWCP_ERR, "Invalid plan id:%d", test_plan_id);
			goto end;
		}
		if (num_iters != -1)
			test_plan->num_test_iters = num_iters;

		if (num_tp_iters != -1)
			test_plan->num_test_plan_iters = num_tp_iters;

		if (test_plan->num_test_plan_iters == 0)
			test_plan->num_test_plan_iters = 1;

		ret = run_test_plan(test_plan, &test_err);
		if (ret != 0) {
			UBWCP_LOG(UBWCP_ERR, "Failed to run %s id:%d", test_plan->name, test_plan_id);
		} else {
			UBWCP_LOG(UBWCP_INFO, "Ran %s id:%d", test_plan->name, test_plan_id);
		}
		free(test_plan);
	}  else {
		goto err;
	}

	delete allocator;
	return 0;

err:
	delete allocator;

	UBWCP_LOG(UBWCP_ERR, "Bad args: %s", CMD_LINE);
end:
	return 0;
}

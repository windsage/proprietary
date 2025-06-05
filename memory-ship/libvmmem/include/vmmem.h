/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VMMEM_H_
#define _VMMEM_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

/*
 * A opaque handle returned by the FindVMBy* family of functions.
 * A VmHandle is valid until the VmMem instance is closed. However,
 * this may change in the future if VM crash recovery is implemented.
 */
typedef int VmHandle;

typedef uint32_t __u32;

#define VMMEM_READ (1U << 0)
#define VMMEM_WRITE (1U << 1)
#define VMMEM_EXEC (1U << 2)
/*
 * A VmHandle and its associated permissions.
 * Valid permissions are any combination of VMMEM_READ/WRITE/EXEC
 */
typedef std::vector<std::pair<VmHandle, uint32_t>> VmPerm;

/*
 * Peripheral VMs vs Guest VMs:
 * Guest VM:
 * An operating system and its associated devices
 *
 * Peripheral VM:
 * A set of devices without an operating system. These devices may be
 * managed by a Guest VM, but the Guest VM will not have access
 * to the memory owned by the peripheral VM unless otherwise granted.
 */


class VmMem {
  public:
    /*
     * Factory function to create an instance.
     */
    static std::unique_ptr<VmMem> CreateVmMem();
    ~VmMem();

    /* Not copyable or movable */
    VmMem(const VmMem&) = delete;
    VmMem& operator=(const VmMem&) = delete;

    /*
     * For a valid dma-buf fd from one of our own heaps, sets @is_exclusive_owner
     * to true if the buffer hasn't been lent / shared or hyp-assigned
     * away, and sets @is_exclusive_owner to 0 otherwise. If an invalid buffer
     * has been passed or if one of the kernel IOCTLs needed to make
     * this inference fails, @is_exclusive_owner dones't hold a valid value. The
     * caller is responsible for checking the return value of this
     * function to infer this.
     *
     * @fd: A dma-buf file descriptor
     * @is_exclusive_owner: A varaible passed in by the caller that will store
     * whether the buffer is secure or not
     * @return: 0 on success, or a non-zero number on failure -
     * pre-5.10 targets will return -ENOTTY
     */
    static int IsExclusiveOwnerDmabuf(int fd, bool &is_exclusive_owner);

    /*
     * Look up a Virtual Machine by name.
     *
     * @name: Must match exactly. No regular expressions.
     * @return: a VmHandle, or a negative number on failure.
     */
    VmHandle FindVmByName(const std::string& name);

    /*
     * Modify which Virtual Machines have access to a dma_buf.
     * The buffer must be exclusively owned by the local VM
     *
     * @dma_buf_fd: A dma_buf file descriptor. Only dma_bufs allocated from
     * QTI dma-buf-heaps are supported.
     * @perms: Refer to @VmPerm. Must not contain the local VM.
     * @memparcel_handle: optional return value. Used as an input arguement
     * to RetrieveDmabuf. Not all VMs support this field; in this case the
     * return value is undefined.
     * @return: 0 upon success, or a negative number on failure.
     */
    int LendDmabuf(int dma_buf_fd,
      const VmPerm& perms,
      int64_t *memparcel_hdl = nullptr);

    /*
     * Should be used instead of LendDmaBuf if the local VM
     * needs to be able to access the dmabuf.
     */
    int ShareDmabuf(int dma_buf_fd,
      const VmPerm& perms,
      int64_t *memparcel_hdl = nullptr);

    /*
     * Import a buffer which was previously Lent or Shared with the local
     * VM by an instance of this library running on remote VM.
     *
     * @owner: The remote VM who Lent or Shared the dmabuf with the local VM.
     * @perms: See VmPerm.
     * If the permissions do not match with the remote VM's call to
     * Lend/Share the call will fail.
     * @memparcel_hdl: Opaque handle from the remote VM's Lend/Share call.
     * The method of transfering the value of this handle from the remote VM
     * to the local VM is outside the scope of this library.
     * @return: A negative number on failure, or a dma-buf file descriptor
     * on success. Full RWX access to this file descriptor will only be
     * available if allowed by @perms.
     */
    int RetrieveDmabuf(VmHandle owner, const VmPerm& perms,
        int64_t memparcel_hdl);

    /*
     * Reclaim a dmabuf which the local VM Lent or Shared with a remote VM.
     * If the remote VM has not released their reference to the dmabuf, this
     * call will fail.
     *
     * Closing the dma-buf fd will have the same effect as calling this
     * function; the former is the recommended approach.
     *
     * Upon success the dma-buf will belong exclusively to the local VM.
     *
     * @fd: A dma-buf file descriptor.
     * @memparcel_handle: Opaque handle obtained from Lend or Share
     * @return: A negative number on failure, 0 on success.
     */
    int ReclaimDmabuf(int fd, int64_t memparcel_hdl);

    /*
     * Allocate a dmabuf from the given heap on the Android VM, and place
     * it in the given heap on the local VM.
     * If the local VM is the Android VM, this call will fail.
     * This API should only be used for the TrustedUI usecase.
     *
     * @size: The size of the allocation.
     * @perms: Refer to @VmPerm.
     * @src_heap_name: The heap the memory will be allocated from on the
     * remote VM.
     * @dst_heap_name: The heap the memory will be inserted into on the
     * local VM.
     * @return: A negative number on failure, or a mem_buf_fd on success.
     * Closing the fd will return the memory to the Remote VM.
     */
    int RemoteAllocDmabuf(uint64_t size, const VmPerm& perms,
        const std::string& src_dma_heap_name,
        const std::string& dst_dma_heap_name);
    /*
     * Allocate a file descriptor which represents a suggestion from
     * userspace that a large amount of memory will be needed in the near
     * future. If the kernel decides to honor this suggestion, there is no
     * guarantee of exclusive access to this memory by the requesting process
     * Closing the file descriptor indicates the memory is no longer required.
     * @size: The requested size of memory to be made free.
     * @name: A unique name for debug purposes.
     */
    int MemorySizeHint(int64_t size, const std::string& name);
  private:
    VmMem();
    int PopulateAcl(struct acl_entry *acl, uint32_t nr_acl_entries,
        const VmPerm &perm);
    int PopulateRemoteAllocData(union RemoteAllocData *data,
        __u32 *mem_type,
        const std::string& dma_heap_name,
        const std::string& ion_heap_name);
    int QueryIonHeaps();
    int DetectBackend();
    /* Ioctl Versions */
    int LendDmabufIoctl(int fd, const VmPerm& dst_perms,
        int64_t *memparcel_hdl);
    int ShareDmabufIoctl(int fd, const VmPerm& dst_perms,
        int64_t *memparcel_hdl);
    int RetrieveDmabufIoctl(VmHandle owner, const VmPerm& perms,
        int64_t memparcel_hdl);
    int ReclaimDmabufIoctl(int fd, int64_t memparcel_hdl);
    int RemoteAllocDmabufIoctl(uint64_t size, const VmPerm& perms,
        const std::string& src_dma_heap_name,
        const std::string& dst_dma_heap_name);
    int MemorySizeHintIoctl(int64_t size, const std::string& name);
  class VM {
    public:
      VM(){}
      ~VM();
      std::string name;
      int id;
      int fd;
      VmHandle handle;
  };
  /*
   * When dma_buf_heaps are not present, memory allocations are expected
   * to be done through ion, which has its own way of allocating secure
   * memory. In this case LendDmabuf() becomes a no-op.
   */
  bool have_dma_buf_heaps;
  int mem_buf_fd;
  std::unordered_map<VmHandle, std::shared_ptr<VM>> handle_map;
};

#endif /*_VMMEM_H_*/

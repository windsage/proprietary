/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <client_fs_utils.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// Utility function for splitting a path into its components
std::vector<std::string> splitPath(std::string path) {

    std::vector<std::string> path_comps;
    std::string delim = "/";
    std::string path_copy(path);
    size_t pos = 0;

    /* Return an empty vector if we have an empty string.
     * A path that is just the root directory ('/') won't have any components,
     * so we can return an empty vector in that case too.
    */
    if (path.empty() || path == delim) {
      return path_comps;
    }

    // Erase first '/' or it will be the first thing string::find returns.
    if (path_copy[0] == delim[0]) {
      path_copy.erase(0, 1);
    }

    /* Make sure we use a trailing '/' so the last path component isn't skipped.
     * This is just for path_copy and it won't be saved to the original string.
     */
    if (path_copy.back() != delim[0]) {
      path_copy.push_back(delim[0]);
    }

    while ((pos = path_copy.find(delim)) != std::string::npos) {
        std::string comp = path_copy.substr(0, pos);
        /* We only want to add path components to the vector, not '/' characters.
         * For example, if we have a path "/x//y/z", we only want to save
         * 'x', 'y', and 'z' to the vector. If we don't check if the
         * substring contains the delimiter, '/' would also be pushed to the vector.
         */
        if (comp != delim) {
          path_comps.push_back(comp);
        }
        path_copy.erase(0, pos + 1);
    }
    return path_comps;
}

Status fs_utils_mkdir(std::string path, std::string &error_msg) {
    // Create intermediate directories if they don't already exist.
    std::vector<std::string> pathComps = splitPath(path);
    std::string currentPath = "/";
    for (auto iter : pathComps) {
        currentPath += iter;
        currentPath += "/";
        if (mkdir(currentPath.c_str(), 0666) < 0 && errno !=  EEXIST) {
            error_msg = std::string(strerror(errno));
            return Status::FAILURE;
        }
    }
    return Status::SUCCESS;
}


dir_check_result_t fs_utils_check_dir(std::string path, std::string &error_msg) {
    struct stat sbuf;
    if (stat(path.c_str(), &sbuf) == 0) {
        error_msg = std::string(strerror(errno));
        if (S_ISDIR(sbuf.st_mode)) {
            return IS_DIR;
        }
        else {
            return IS_FILE;
        }
    }
    else {
        return DNE;
    }
}
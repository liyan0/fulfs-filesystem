#include "filesystem.h"
#include "block.h"
#include "inode.h"
#include "superblock.h"
#include "data_block.h"

#include "../utils/math.h"
#include "../utils/log.h"
#include "../memory/alloc.h"


fulfs_errcode_t fulfs_format(device_handle_t device, int sectors_per_block)
{
    block_no_t block_count = device_section_count(device) / sectors_per_block;
    block_no_t inode_table = 1;

    /* inode 所占的block数 */
    int inode_blocksize =  inode_block_count(sectors_per_block * BYTES_PER_SECTOR, INODE_MAX_COUNT);

    block_no_t data_block = inode_table + inode_blocksize;


    if (block_count  <= 0 ||
        data_block >= block_count) {
        log_debug("空间太小无法格式化");
        return FULFS_FAIL;
    }


    /* 初始化磁盘的inode区 */
    dev_inode_ctrl_t dev_inode_ctrl;
    dev_inode_ctrl_init(&dev_inode_ctrl, device, sectors_per_block * BYTES_PER_SECTOR, inode_table, inode_blocksize);

    inode_t inode;
    inode_init(&inode);

    for (inode_no_t i = 0; i < INODE_MAX_COUNT; i++) {
        if (!inode_dump(&dev_inode_ctrl, i, &inode)) {
            log_debug("inode写入失败: %d号设备, %d号inode", device, (int)i);
            return FULFS_FAIL;
        }
    }

    /* 初始化磁盘的data block区 */
    block_no_t data_block_free_stack;
    bool success = data_blocks_init(device, sectors_per_block, data_block, block_count - data_block, &data_block_free_stack);
    if (!success) {
        log_debug("data_block区初始化失败: %d号设备", device);
        return FULFS_FAIL;
    }

    superblock_t sb;
    superblock_create(&sb, device_section_count(device), sectors_per_block,
                      inode_table, data_block, data_block_free_stack);

    /* 写入superblock */
    if (!superblock_dump(device, &sb)) {
        log_debug("superblock写入失败: %d号设备", device);
        return FULFS_FAIL;
    }

    return FULFS_SUCCESS;
}

bool fulfs_filesystem_init(fulfs_filesystem_t* fs, device_handle_t device)
{
    return superblock_load(device, &fs->sb);
}

fulfs_filesystem_t* fulfs_filesystem_new(device_handle_t device)
{
    fulfs_filesystem_t* fs_ctrl = FT_NEW(fulfs_filesystem_t, 1);
    if (fs_ctrl == NULL) {
        return false;
    }

    if (!fulfs_filesystem_init(fs_ctrl, device)) {
        return false;
    }

    return fs_ctrl;
}

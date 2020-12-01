#include "ssufs-ops.h"

extern struct filehandle_t file_handle_array[MAX_OPEN_FILES];

int ssufs_allocFileHandle() {
	for(int i = 0; i < MAX_OPEN_FILES; i++) {
		if (file_handle_array[i].inode_number == -1) {
			return i;
		}
	}
	return -1;
}

int ssufs_create(char *filename){
	/* 1 */
	int i_num;
	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
	//해당 파일이 존재하면 return -1;
	if(open_namei(filename) != -1)
		return -1;
	i_num = ssufs_allocInode();
	printf("create return %d\n", i_num);
	printf("status : %d, name : %s, size : %d\n", inode->status, inode->name, inode->file_size);
	ssufs_readInode(i_num, inode);
	//inode의 상태를 사용중으로 바꿈
	inode->status = INODE_IN_USE;
	//해당 inode의 이름을 저장해줌
	for(int i=0; i<strlen(filename); i++)
		inode->name[i] = filename[i];
	//해당 inode의 size를 저장해줌
	inode->file_size = 256;
		for (int i = 0; i < MAX_FILE_SIZE; i++)
	{
		if (inode->direct_blocks[i] != -1)
		{
			ssufs_freeDataBlock(inode->direct_blocks[i]);
		}
		inode->direct_blocks[i] = -1;
	}
	ssufs_writeInode(i_num, inode);
	
	return i_num;
	
}

void ssufs_delete(char *filename){
	/* 2 */
	int i_num;
	//해당 파일의 inode num을 찾아서 리턴
	i_num = open_namei(filename);
	//삭제를 요청한 파일이 없으면 종료
	if(i_num == -1)
		return;
	ssufs_freeInode(i_num);

	
}

int ssufs_open(char *filename){
	/* 3 */
	int i_num;
	if(open_namei(filename) == -1){
		printf("open error\n");
		return -1;
	}
	i_num = ssufs_allocFileHandle();
	file_handle_array[i_num].offset = 0;
//	printf("open return %d\n", i_num);
	return i_num;
}

void ssufs_close(int file_handle){
	file_handle_array[file_handle].inode_number = -1;
	file_handle_array[file_handle].offset = 0;
}

int ssufs_read(int file_handle, char *buf, int nbytes){
	/* 4 */
}

int ssufs_write(int file_handle, char *buf, int nbytes){
	/* 5 */
}

int ssufs_lseek(int file_handle, int nseek){
	int offset = file_handle_array[file_handle].offset;

	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);
	
	int fsize = tmp->file_size;
	
	offset += nseek;

	if ((fsize == -1) || (offset < 0) || (offset > fsize)) {
		free(tmp);
		return -1;
	}

	file_handle_array[file_handle].offset = offset;
	free(tmp);

	return 0;
}

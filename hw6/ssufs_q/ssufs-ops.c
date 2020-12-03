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
	if(i_num == -1)
		return -1;
	//printf("create return %d\n", i_num);
	//printf("status : %d, name : %s, size : %d\n", inode->status, inode->name, inode->file_size);
	ssufs_readInode(i_num, inode);
	//inode의 상태를 사용중으로 바꿈
	inode->status = INODE_IN_USE;
	//해당 inode의 이름을 저장해줌
	strcpy(inode->name, filename);
	ssufs_writeInode(i_num, inode);
	free(inode);

	return i_num;
}

void ssufs_delete(char *filename){
	/* 2 */
	int f_inode;
	//char *init = (char*)malloc(sizeof(char)*BLOCKSIZE);
	char temp[64];
	//	printf("!!!!!!%ld\n", sizeof(init));
	//memset(init, 0, BLOCKSIZE);
	
	struct inode_t *tmp = (struct inode_t *)malloc(sizeof(struct inode_t));
	
	//해당 파일의 inode num을 찾아서 리턴
	f_inode = open_namei(filename);
	//삭제를 요청한 파일이 없으면 종료
	if(f_inode == -1)
		return;
	
	ssufs_readInode(f_inode, tmp);
	
	
	for(int i = 0 ; i<MAX_FILE_SIZE; i++){
		
		if(tmp->direct_blocks[i] != -1){
			
			//ssufs_freeDataBlock(tmp->direct_blocks[i]);
			//printf("%s is deleting---------- [%s]\n", filename, init);
			ssufs_readDataBlock(tmp->direct_blocks[i], temp);
			printf("delete index[%d] : %s\n", i, temp);
			memset(temp, 0, sizeof(temp));
			printf("delete check : %s\n", temp);
			ssufs_writeDataBlock(tmp->direct_blocks[i], temp);
			
		}
	}
	ssufs_freeInode(f_inode);
	free(tmp);
}

int ssufs_open(char *filename){
	/* 3 */
	int i_num;
	int handler_num;
	//해당하는 파일의 inode 번호를 받아온다.
	i_num = open_namei(filename);
	//현재 사용가능한 filehandler의 인덱스를 받아옴
	handler_num = ssufs_allocFileHandle();
	//에러처리
	if(i_num == -1 || handler_num == -1){
		return -1;
	}
	file_handle_array[handler_num].offset = 0;
	//filehandler에 inode number 저장
	file_handle_array[handler_num].inode_number = i_num;
	//printf("inode : %d, file handler : %d\n", i_num, handler_num);
	return handler_num;
}

void ssufs_close(int file_handle){
	file_handle_array[file_handle].inode_number = -1;
	file_handle_array[file_handle].offset = 0;
}

int ssufs_read(int file_handle, char *buf, int nbytes){
	/* 4 */
	int f_offset = file_handle_array[file_handle].offset;
	int f_inode = file_handle_array[file_handle].inode_number;
	
	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(f_inode, tmp);
	
	if(f_offset + nbytes > tmp->file_size)
		return -1;

	int curr =0;
	int consume = nbytes;
	char tmp_buf[64];
	for(int i=0; i<MAX_FILE_SIZE; i++){
		if( 0 <= f_offset && f_offset < BLOCKSIZE){
			ssufs_readDataBlock(tmp->direct_blocks[i], tmp_buf);
			while(f_offset < BLOCKSIZE){
				 buf[curr++] = tmp_buf[f_offset++];
				consume--;
			}
			//offset 초기화 해버림
			f_offset =0;
			if(consume ==0)
				break;
		
		}
		if(f_offset != 0){
			f_offset-=BLOCKSIZE;
			//printf("오프셋 감소 %d\n", f_offset);
		}
		//for(int j=0; j<)
	}

	ssufs_lseek(file_handle, nbytes);
	free(tmp);

	return 0;
	
}

int ssufs_write(int file_handle, char *buf, int nbytes){
	/* 5 */
	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	int f_offset = file_handle_array[file_handle].offset;
	int f_inode = file_handle_array[file_handle].inode_number;
	int curr =0;
	int consume = nbytes;
	char tmp_buf[BLOCKSIZE];
	char backup[BLOCKSIZE];
	int start_index;
	int isUsed[MAX_FILE_SIZE]= {-1, -1, -1, -1};

	ssufs_readInode(f_inode, tmp);
	//예외처리
	if((tmp->file_size + nbytes) > MAX_FILE_SIZE*BLOCKSIZE)
		return -1;

	for(int i=0; i<MAX_FILE_SIZE; i++){
		if( 0 <= f_offset && f_offset < BLOCKSIZE){
			//printf("오프셋 확인 : %d\n", f_offset);
			if(tmp->direct_blocks[i] == -1){
				tmp->direct_blocks[i] = ssufs_allocDataBlock();
				//여기다가 롤백 처리하면 될듯
				if(tmp->direct_blocks[i] == -1){
					for(int j=0; j<MAX_FILE_SIZE; j++){
						if(isUsed[j] ==1){
							if(j == start_index)
								ssufs_writeDataBlock(tmp->direct_blocks[j], backup);
							else{
							//	ssufs_writeDataBlock(tmp->direct_blocks[j], NULL);
								ssufs_freeDataBlock(tmp->direct_blocks[j]);
							}
						}
					}
					return -1;
				}
			}
			if(f_offset != 0){
				ssufs_readDataBlock(tmp->direct_blocks[i], tmp_buf);
				strcpy(backup , tmp_buf);
				start_index = i;
				//printf("backup : %s\n", backup);
			}
			while(f_offset < BLOCKSIZE){
				tmp_buf[f_offset++] = buf[curr++];
				consume--;
			}
			//offset 초기화 해버림
			f_offset =0;
			ssufs_writeDataBlock(tmp->direct_blocks[i], tmp_buf);
			isUsed[i] = 1;
			if(consume ==0)
				break;
	
		}
		if(f_offset != 0)
			f_offset-=BLOCKSIZE;
			
	}

	tmp->file_size += nbytes;
	ssufs_writeInode(f_inode, tmp);
	ssufs_lseek(file_handle, nbytes);
	free(tmp);
	return 0;
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

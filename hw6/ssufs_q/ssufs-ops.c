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
	//해당 파일이 이미 존재하면 return -1;
	if(open_namei(filename) != -1)
		return -1;
	i_num = ssufs_allocInode();
	if(i_num == -1)
		return -1;
	//inode를 읽어와서 상태 변경후 써줌
	ssufs_readInode(i_num, inode);
	inode->status = INODE_IN_USE;
	strcpy(inode->name, filename);
	ssufs_writeInode(i_num, inode);
	free(inode);

	return i_num;
}

void ssufs_delete(char *filename){
	/* 2 */
	int f_inode;
	char *buf = (char*)malloc(sizeof(char)*(BLOCKSIZE));
	struct inode_t *tmp = (struct inode_t *)malloc(sizeof(struct inode_t));
	
	//해당 파일의 inode num을 찾아서 리턴
	f_inode = open_namei(filename);
	//삭제를 요청한 파일이 없으면 종료
	if(f_inode == -1)
		return;
	//삭제할 파일의 inode 정보를 읽어옴
	ssufs_readInode(f_inode, tmp);
	for(int i = 0 ; i<MAX_FILE_SIZE; i++){
		//사용했던 데이터 블럭을 지워줌
		if(tmp->direct_blocks[i] != -1){
			ssufs_readDataBlock(tmp->direct_blocks[i], buf);
			memset(buf, 0, sizeof(buf));
			ssufs_writeDataBlock(tmp->direct_blocks[i], buf);
		}
	}
	//해당 inode 해제
	ssufs_freeInode(f_inode);
	free(tmp);
	free(buf);
}

int ssufs_open(char *filename){
	/* 3 */
	int i_num;
	int handler_num;
	//해당하는 파일의 inode 번호를 받아온다.
	i_num = open_namei(filename);
	//현재 사용가능한 filehandler의 인덱스를 받아옴
	handler_num = ssufs_allocFileHandle();
	//해당하는 파일이 없거나 fileHadnler를 할당받지 못했을때 -1 리턴
	if(i_num == -1 || handler_num == -1){
		return -1;
	}
	//file_handler 정보 업데이트
	file_handle_array[handler_num].offset = 0;
	file_handle_array[handler_num].inode_number = i_num;

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
	int curr =0;
	int consume = nbytes;

	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(f_inode, tmp);
	
	if(f_offset + nbytes > tmp->file_size)
		return -1;

	char tmp_buf[BLOCKSIZE];
	for(int i=0; i<MAX_FILE_SIZE; i++){
		if( 0 <= f_offset && f_offset < BLOCKSIZE){
			ssufs_readDataBlock(tmp->direct_blocks[i], tmp_buf);
			while(f_offset < BLOCKSIZE){
				buf[curr++] = tmp_buf[f_offset++];
				consume--;
				if(consume == 0){
					buf[curr] = 0;
					ssufs_lseek(file_handle, nbytes);
					free(tmp);
					return 0;
				}
			}
			//offset 초기화 해버림
			f_offset =0;
		}
		if(f_offset != 0)
			f_offset-=BLOCKSIZE;

	}
	//여기까지 내려오면 뭔가 잘못된거임
	free(tmp);
	return -1;
}

int ssufs_write(int file_handle, char *buf, int nbytes){
	/* 5 */
	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	int f_offset = file_handle_array[file_handle].offset;
	int f_inode = file_handle_array[file_handle].inode_number;
	int curr =0;
	char *tmp_buf = (char*)malloc(sizeof(char)*(BLOCKSIZE));
	char *rollback = (char*)malloc(sizeof(char)*(BLOCKSIZE));
	int rollback_index;
	int isUsed[MAX_FILE_SIZE]= {-1, -1, -1, -1};

	ssufs_readInode(f_inode, tmp);
	//예외처리
	if((tmp->file_size + nbytes) > MAX_FILE_SIZE*BLOCKSIZE)
		return -1;

	for(int i=0; i<MAX_FILE_SIZE; i++){
		if( 0 <= f_offset && f_offset < BLOCKSIZE){
			if(tmp->direct_blocks[i] == -1){

				tmp->direct_blocks[i] = ssufs_allocDataBlock();
				if(tmp->direct_blocks[i] == -1){
					for(int j=0; j<MAX_FILE_SIZE; j++){
						if(isUsed[j] == 1){
							if(j == rollback_index)
								ssufs_writeDataBlock(tmp->direct_blocks[j], rollback);
							else{
								ssufs_writeDataBlock(tmp->direct_blocks[j], tmp_buf);
								ssufs_freeDataBlock(tmp->direct_blocks[j]);
							}
						}
					}
					free(rollback);
					free(tmp_buf);
					free(tmp);
					return -1;
				}
			}
			//블럭 중간부터 write를 해야할때 해당 블럭을 읽어온다.
			if(f_offset != 0){
				ssufs_readDataBlock(tmp->direct_blocks[i], tmp_buf);
				//rollback을 대비하여 정보 저장
				strcpy(rollback , tmp_buf);
				rollback_index = i;
			}
			while(f_offset < BLOCKSIZE){
				tmp_buf[f_offset++] = buf[curr++];
				if(curr  == nbytes){
					ssufs_writeDataBlock(tmp->direct_blocks[i], tmp_buf);
					tmp->file_size += nbytes;
					ssufs_writeInode(f_inode, tmp);
					ssufs_lseek(file_handle, nbytes);
					free(tmp_buf);
					free(tmp);
					return 0;
				}
			}
			//offset 초기화 해버림
			ssufs_writeDataBlock(tmp->direct_blocks[i], tmp_buf);
			f_offset =0;
			isUsed[i] = 1;
			memset(tmp_buf, 0, BLOCKSIZE);
		}
		if(f_offset != 0)
			f_offset-=BLOCKSIZE;
	}
	free(tmp_buf);
	free(rollback);
	free(tmp);
	return -1;
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

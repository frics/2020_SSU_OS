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
	int i_num;
	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
	
	//해당 파일의 inode num을 찾아서 리턴
	i_num = open_namei(filename);

	//삭제를 요청한 파일이 없으면 종료
	if(i_num == -1)
		return;
	ssufs_freeInode(i_num);
	free(inode);
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
	int d;
	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);

	if(file_handle_array[file_handle].offset + nbytes > tmp->file_size)
		return -1;
	if(file_handle_array[file_handle].offset % BLOCKSIZE != 0 )
		d = file_handle_array[file_handle].offset / BLOCKSIZE;
	else if(file_handle_array[file_handle].offset / BLOCKSIZE >0)
		d = file_handle_array[file_handle].offset / BLOCKSIZE -1;
	else
		d = file_handle_array[file_handle].offset / BLOCKSIZE;
	printf("d : %d\n", d);

	for(int i=d; i<MAX_FILE_SIZE; i++){
		if(tmp->direct_blocks[i] != -1){
			//printf("direct_blocks(read) : %d\n", tmp->direct_blocks[i]);
			ssufs_readDataBlock(tmp->direct_blocks[i], buf);
		}	
	}
	return 1;
	
}

int ssufs_write(int file_handle, char *buf, int nbytes){
	/* 5 */
	int offset = file_handle_array[file_handle].offset;
	int curr = offset;
	int i_num = file_handle_array[file_handle].inode_number;
	//1개 데이터 블럭당 64byte 
	int buf_size =0;
	//offset 위치한 인덱스를 찾기 위해 선언
	int start_index;
	//write도중 오류가 있을때 되돌리기 위해 임시 저장 배열
	int tmp_dalloc[MAX_FILE_SIZE];
	//임시 저장 배열 초기화
	memset(tmp_dalloc, -1, sizeof(tmp_dalloc));
	//inode구조체 선언
	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	//file handler에 저장된 inode 번호에 해당하는 inode 정보를 받아온다.
	ssufs_readInode(i_num, tmp);
	int limit = BLOCKSIZE*MAX_FILE_SIZE;
	//파일이 가질 수 있는 최대 크기를 넘으면 -1 반환
	if( tmp->file_size+ nbytes > limit || (offset + nbytes) > limit )
		return -1;
	
	//시작 인덱스 지정
	for(int i=0; i<MAX_FILE_SIZE; i++){
		if(offset >= (i*64) && offset < ((i+1)*BLOCKSIZE)){
			start_index = i;
			break;
		}
		curr -= BLOCKSIZE;
	}

	//buffer preprocess
	int n = (curr + nbytes)/BLOCKSIZE;
	char tmp_buf[n][BLOCKSIZE];
	int write = nbytes;

	if(curr != 0){
		ssufs_readDataBlock(start_index, tmp_buf[start_index]);
		int j = 0;
		int block = start_index;
		
			for(int i =curr; i<BLOCKSIZE; i++){
				tmp_buf[block][i] = buf[j++];
				write--;
			}
			curr = 0;
			block++;
		
		/*for(int i = start_index; i<MAX_FILE_SIZE; i++){
			printf("string : %s\n",tmp_buf[i] );
		}*/
	}else{

	}
	

	printf("start index : %d, offest : %d\n", start_index, curr);
	//buffer preprocessing
	/*int n = nbytes/BLOCKSIZE;
	if(nbytes % BLOCKSIZE != 0)
		n++;
	char tmp_buf[n][BLOCKSIZE];
	
	for(int i=0; i<n; i++){
		for(int j=i*BLOCKSIZE; j<BLOCKSIZE; j++){
			tmp_buf[i][j] = buf[j];
		}
	}
	*/

	for(int j=start_index; j<MAX_FILE_SIZE; j++){
		if(buf_size>=nbytes)
			break;
		if(tmp->direct_blocks[j] == -1){
			tmp_dalloc[j] = ssufs_allocDataBlock();
			tmp->direct_blocks[j] = tmp_dalloc[j];
			if(tmp_dalloc[j] == -1){
				for(int k=0; k<MAX_FILE_SIZE; k++){
					if(tmp->direct_blocks[k] != -1)
						tmp->direct_blocks[k] = -1;
				}
				return -1;
			}
		}
		//printf("direct block(write) : %d\n", tmp->direct_blocks[j]);
		//offset - start_index*BLOCKSIZE;
		//printf("index : %d, offest : %d\n", j, pos);
		int adr = tmp->direct_blocks[j] + curr;
		ssufs_writeDataBlock(adr, buf);
		buf_size+=BLOCKSIZE;
	}
	tmp->file_size += nbytes;
	ssufs_writeInode(file_handle_array[file_handle].inode_number, tmp);
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

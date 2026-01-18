/**
 * @file    main.c
 * @author  shark(yshark@qq.com)
 * @brief   
 * @version 0.1
 * @date    2023-05-07
 * 
 * @copyright Copyright (c) 2023..
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#define READ_LEN    (1024)  /*读取长度*/
#define FILL    (uint8_t)0xff   /*填充字符*/

uint8_t read_buf[READ_LEN];/*读取缓冲*/
uint8_t fill_byte = FILL;/*填充字节*/
uint8_t fill_array[READ_LEN] = {[0 ... READ_LEN-1] = FILL};/*填充数组*/
char output_filename[128];


int cmp(const void* a, const void* b);
int main(int argc,char *argv[])
{
#ifdef DEBUG
argc = 12;
argv[1] = "C:/Users/weikang/Desktop/C/bin_file_merge/a.bin";
argv[2] = "C:/Users/weikang/Desktop/C/bin_file_merge/b.bin";
argv[3] = "C:/Users/weikang/Desktop/C/bin_file_merge/c.bin";
argv[4] = "C:/Users/weikang/Desktop/C/bin_file_merge/out.bin";
argv[5] = "C:/Users/weikang/Desktop/C/bin_file_merge/out1.bin";
argv[6] = "0x2000";
argv[7] = "0x4000";
argv[8] = "0x6000";
argv[9] = "0x8000";
argv[10] = "0xa000";
argv[11] = "C:/Users/weikang/Desktop/C/bin_file_merge/a_outPut.bin";
#endif
    if(argc <= 4)
    {
        printf("Please enter at least two file names");
        return -1;
    }
    if(argc%2)
    {
        printf("参数缺少,需要为每个bin文件指定偏移地址,并且需要指定一个输出文件名\n");
        return -1;
    }


    uint8_t file_num = (argc-2)/2;
    int32_t offset[file_num];
    int32_t offset_sort[file_num];
    FILE *file[file_num];
    char *filename[file_num];
    char *sort_filename[file_num];

    memset(offset,0,sizeof(offset));
    memset(offset_sort,0,sizeof(offset_sort));
    memset(file,0,sizeof(file));
    memset(filename,0,sizeof(filename));
    strcpy(output_filename,argv[argc-1]);
    for(int i = 0;i < file_num;i++)
    {
        char* endptr;
        filename[i] = argv[i+1];
        offset[i] = strtoimax(argv[i+1+file_num],&endptr,0);
    }
    memcpy(offset_sort,offset,sizeof(offset));
    qsort(offset_sort,sizeof(offset_sort)/sizeof(uint32_t),sizeof(uint32_t),cmp);
    for(int i = 0;i < sizeof(offset_sort)/sizeof(uint32_t);i++)
    {
        for(int j = 0;j < sizeof(offset_sort)/sizeof(uint32_t);j++)
        {
            if(offset_sort[i] == offset[j])
            {
                sort_filename[i] = filename[j];
                break;
            }
        }
    }
    
    FILE *fp_save = fopen(output_filename,"wb+"); 
    if(fp_save == NULL)
    {
        perror("fopen fail");
        return -1;
    }

    for(int i = 0;i < file_num;i++)
    {
        file[i] = fopen(sort_filename[i],"rb");
        if(file[i] == NULL)
        {
            printf("open %s failed : %s\n", sort_filename[i],strerror(errno));
            remove(output_filename);
            return -1;            
        }

uint32_t file_size = 0;
long tell = ftell(fp_save);
        /*填充*/
        while((offset_sort[i] - ftell(fp_save)) >= READ_LEN)
        {
            tell = ftell(fp_save);
            file_size = offset_sort[i] - tell;
            uint16_t write_len = fwrite(fill_array,sizeof(char),READ_LEN,fp_save);
        }
        tell = ftell(fp_save);
        while(offset_sort[i] - ftell(fp_save) > 0)
        {
            tell = ftell(fp_save);
            uint16_t write_len = fwrite(&fill_byte,sizeof(char),1,fp_save);
        }
        /*复制*/
        fseek(fp_save, offset_sort[i], SEEK_SET);
        while(1)
        {
            uint16_t read_len = fread(read_buf,sizeof(char),READ_LEN,file[i]);
            uint16_t write_len = fwrite(read_buf,sizeof(char),read_len,fp_save);
            if(write_len != read_len)
            {
                perror("fwrite fail");
                fclose(file[i]);
                remove(output_filename);
                return -1;                 
            }
            if(read_len < READ_LEN)
            {
                fclose(file[i]);
                break;    
            }        
        }
        

    }



    fclose(fp_save);

    for(int i = 0;i < file_num;i++)
    {
        printf("%#010X \t%s\n",offset_sort[i],sort_filename[i]);
    }

    return 0;
}



/**
 * @brief   获取文件大小
 * 
 * @param   stream      
 * @return  uint64_t -1：获取失败 其他:获取成功
 */
 uint64_t get_file_size(FILE *stream)
{
	uint64_t file_size = -1;
	uint64_t cur_offset = ftell(stream);	// 获取当前偏移位置
	if (cur_offset == -1) {
		printf("ftell failed :%s\n", strerror(errno));
		return -1;
	}
    if (fseek(stream, 0, SEEK_END) != 0) {	// 移动文件指针到文件末尾
		printf("fseek failed: %s\n", strerror(errno));
		return -1;
	}
	file_size = ftell(stream);	// 获取此时偏移值，即文件大小
	if (file_size == -1) {
		printf("ftell failed :%s\n", strerror(errno));
	}
    if (fseek(stream, cur_offset, SEEK_SET) != 0) {	// 将文件指针恢复初始位置
		printf("fseek failed: %s\n", strerror(errno));
		return -1;
	}
	return file_size;
}

int cmp(const void* a, const void* b)
{
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
 
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
 
    // return (arg1 > arg2) - (arg1 < arg2); // 可行的简写
    // return arg1 - arg2; // 错误的简写（若给出 INT_MIN 则会失败）
}

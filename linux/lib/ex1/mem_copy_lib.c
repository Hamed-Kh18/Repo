#include <stdio.h>
#include <stdint.h>
#include<string.h>


int MEM_COPY(void* dst, void* src , size_t byte_count)
{
    if (dst == NULL || src == NULL) {
        printf("Error: Invalid argumant");
        return -1;
    }
    char* byte_dst_pos = (char*) dst;
    char* byte_dst_end = (char*)dst + byte_count;
    char* byte_src_pos = (char*)src;

    for (; byte_dst_pos < byte_dst_end ; byte_dst_pos++ , byte_src_pos++){
        *byte_dst_pos = *byte_src_pos;
    }

    return 0;
}

int str_alloc_insert(
    char* dst,
    char* ref,
    char* src,
    size_t insertion_offset,
    size_t insertion_len
) {
    if (src == NULL || dst == NULL || ref == NULL) {
        printf("Error: Invalid argumant");
        return -1;
    }

    size_t dst_len = strlen(ref) + insertion_len;
    dst = (char*)malloc(dst_len + 1);
    
    for (size_t i = 0; i < insertion_offset; i++) {
        dst[i] = ref[i];
    }
    
    for (size_t i = 0 ; i < insertion_len; i++) {
        dst[insertion_offset + i] = src[i];
    }

    for (size_t i = 0; i < dst_len - (insertion_offset + insertion_len) ; i++) {
        dst[insertion_offset + insertion_len + i] = ref[insertion_offset + i];
    }

    return 0;
}




int mem_concat(void* dst, void* src)
{
    if (dst == NULL || src == NULL) {
        printf("Error: Invalid argumant");
        return -1;
    }
    size_t dst_len = strlen(dst);
    size_t src_len = strlen(src);
    char* byte_dst_end = ((char*)dst) + dst_len;
    char* byte_src_pos = (char*)src;

    for (size_t i = 0; i < src_len; i++){
        *byte_dst_end = *byte_src_pos;
        //*byte_dst_end++ ;
        //*byte_src_pos++ ;
    }

    return 0;
}

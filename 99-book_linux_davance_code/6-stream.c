#include <stdio.h>
#include <stdlib.h>

void pr_stdio(const char *, FILE *);
int is_unbuffered(FILE *);
int is_linebuffered(FILE *);
int buffer_size(FILE *);

int main(void) {
    FILE *fp;
    fputs("enter any character\n", stdout);
    if (getchar() == EOF)
        perror("getchar error");
    
    fputs("one line to standard error\n", stderr);
    pr_stdio("stdin", stdin);
    pr_stdio("stdout", stdout);
    pr_stdio("stderr", stderr);
    
    if ((fp = fopen("/etc/passwd", "r")) == NULL)
        perror("fopen error");
    
    if (getc(fp) == EOF)
        perror("getc error");
    
    pr_stdio("/etc/passwd", fp);
    exit(0);
}

void pr_stdio(const char *name, FILE *fp) {
    printf("stream = %s, ", name);
    
    if (is_unbuffered(fp))
        printf("unbuffered");
    else if (is_linebuffered(fp))
        printf("line buffered");
    else
        printf("fully buffered");
    
    printf(", buffer size = %ld\n", buffer_size(fp));
}

int is_unbuffered(FILE *fp) {
#if defined(_IO_UNBUFFERED)
    return (fp->_flags & _IO_UNBUFFERED);
#elif defined(_SNBF)
    return (fp->_flags & _SNBF);
#elif defined(_IONBF)
    return (fp->_flags & _IONBF);
#else
    #error unknown stdio implementation!
#endif
}

int is_linebuffered(FILE *fp) {
#if defined(_IO_LINE_BUF)
    return (fp->_flags & _IO_LINE_BUF);
#elif defined(__SLBF)
    return (fp->_flags & __SLBF);
#elif defined(_IOLBF)
    return (fp->_flags & _IOLBF);
#else
    #error unknown stdio implementation!
#endif
}

int buffer_size(FILE *fp) {
#if defined(_IO_UNBUFFERED) || defined(_IO_LINE_BUF)
    return (fp->_IO_buf_end - fp->_IO_buf_base);
#elif defined(_IONBF)
    #ifdef _LP64
        return (fp->_base - fp->_ptr);
    #else
        return (BUFSIZ);
    #endif
#else
    #error unknown stdio implementation!
#endif
}
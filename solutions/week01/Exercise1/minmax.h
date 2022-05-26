/*
(The bit with the question mark and the colon and the other stuff is a fancy if-else shorthand which you hopefully recognize from JAVA.)
https://stackoverflow.com/questions/19846570/macros-c-programming-define-mina-b-a-b-a-b
*/
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MINIDX(oldidx, i, arr) (((arr[oldidx]) < (arr[i]))?(oldidx):(i))
#define MAXIDX(oldidx, i, arr) (((arr[oldidx]) > (arr[i]))?(oldidx):(i))

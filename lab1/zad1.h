// struct Entry {
//   int lines;
//   int words;
//   int chars;
//   char* name;
// };

void create_table(int size);
int count(char** files, int size);
int find_free_block();
void remove_block(int index);

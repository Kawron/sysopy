void create_table(int size);
int count(char** files, int size);
int find_free_block();
void remove_block(int index);
void free_memory();
char* alocate_memory(int index);

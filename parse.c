#include "parse.h"
#include "externs.h"
#include "retriever/retriever.h"

// definitions

#define OPT_MOUNT "-m"
#define OPT_MOUNT_FULL "--mount"

#define OPT_REPOS "-r"
#define OPT_REPOS_FULL "--repositories"

#define OPT_FULL "-f"
#define OPT_FULL_FULL "--full"

#define OPT_NECESSARY "-n"
#define OPT_NECESSARY_FULL "--necessary"

#define OPT_CACHING "-c"
#define OPT_CACHING_FULL "--caching"

#define OPT_DIR "-d"
#define OPT_DIR_FULL "--directory"

#define OPT_LAST "-l"
#define OPT_LAST_FULL "--last"

#define OPT_VERSION "-v"
#define OPT_VERSION_FULL "--version"

#define isOption(string) ((string[0] == '-') ? 1 : 0)

// private functions

int set_mount(int argc, char **argv, int *index){
    
    if (((*index) + 1 >= argc) || (isOption(argv[(*index) + 1]) == 1))
		return -1;
    if (mount != NULL)
		return -1;
	
    if (gstrcpy(&mount, argv[(*index) + 1]) != 0)
    	return -1;
    (*index) += 1;
#ifdef DEBUG
	printf("[Function: set_mount] Mount point set to %s;\n", mount);
#endif
        
    return 0;
    
};

int set_repos(int argc, char **argv, int *index){

    int i = 0;

    if ((*index + 1 >= argc) || (isOption(argv[*index + 1]) == 1))
		return -1;
    if (repo_count != 0)
		return -1;
	
    for (i = *index + 1; (i < argc) && (isOption(argv[i]) != 1); i++)
		repo_count++;
    repos = calloc(repo_count, sizeof(char *));
    rev_count = calloc(repo_count, sizeof(int));
    repo_count = 0;
    for (i = *index + 1; (i < argc) && (isOption(argv[i]) != 1); i++){
		gstrcpy(&repos[repo_count], argv[i]);
		repo_count++;
    };
    *index += repo_count;

    return 0;
    
};

int set_caching(int argc, char **argv, int *index){

	if ((*index + 1 >= argc) || (isOption(argv[*index + 1]) == 1))
		return -1;
	cache_limit = atoi(argv[*index + 1]);
	if (cache_limit < 0)
		return -1;
	*index += 1;

	return 0;

};

int set_directory(int argc, char **argv, int *index){

#define set_directory_finish(value) {				\
			if (temp != NULL)						\
				free(temp);							\
			return value;							\
		}

	struct stat *temp = NULL;

	if ((*index + 1 >= argc) || (isOption(argv[*index + 1]) == 1))
		set_directory_finish(-1);
	temp = single(struct stat);
	if ((stat(argv[*index + 1], temp) != 0) || ((temp->st_mode & S_IFDIR) == 0))
		set_directory_finish(-1);
	*index += 1;
	if (gstrcpy(&tmp_dir, argv[*index]) != 0)
		set_directory_finish(-1);
	set_directory_finish(0);
	
};

void parse_option(int argc, char **argv, int *index){

    if ((strcmp(argv[(*index)], OPT_MOUNT) == 0) || (strcmp(argv[(*index)], OPT_MOUNT_FULL) == 0)){
		if (set_mount(argc, argv, index) != 0)
			fail(ERR_PARAMETRES);
	}
    else if ((strcmp(argv[*index], OPT_REPOS) == 0) || (strcmp(argv[*index], OPT_REPOS_FULL) == 0)){
		if (set_repos(argc, argv, index) != 0)
			fail(ERR_PARAMETRES);
    }
    else if ((strcmp(argv[*index], OPT_FULL) == 0) || (strcmp(argv[*index], OPT_FULL_FULL) == 0))
    	structure = STRUCTURE_FULL;
    else if ((strcmp(argv[*index], OPT_NECESSARY) == 0) || (strcmp(argv[*index], OPT_NECESSARY_FULL) == 0))
    	structure = STRUCTURE_NECESSARY;
    else if ((strcmp(argv[*index], OPT_LAST) == 0) || (strcmp(argv[*index], OPT_LAST_FULL) == 0))
    	layout = LAYOUT_LAST;
    else if ((strcmp(argv[*index], OPT_CACHING) == 0) || (strcmp(argv[*index], OPT_CACHING_FULL) == 0)){
    	if (set_caching(argc, argv, index) != 0)
    		fail(ERR_PARAMETRES);
    }
    else if ((strcmp(argv[*index], OPT_DIR) == 0) || (strcmp(argv[*index], OPT_DIR_FULL) == 0)){
    	if (set_directory(argc, argv, index) != 0)
    		fail(ERR_PARAMETRES);
    }
    else if ((strcmp(argv[*index], OPT_VERSION) == 0) || (strcmp(argv[*index], OPT_VERSION_FULL) == 0))
		printf("archfs - filesystem in userspace for rdiff-backup repositories; version %s\n", ARCHFS_VERSION);
    else 
		fail(ERR_UNKNOWN_OPTION);

};

void parse_repo(int argc, char** argv, int *index){

    int i = 0;

    if (repo_count != 0)
		fail(ERR_PARAMETRES);
    for (i = *index; (i < argc) && (!isOption(argv[i])); i++){ };
    repo_count = i - *index;
    repos = calloc(repo_count, sizeof(char *));
    for (i = *index; (i < argc) && (!isOption(argv[i])); i++)
		gstrcpy(&repos[i - *index], argv[i]);
    *index += repo_count - 1;
    
};

void parse_mount(char *arg){

    if (mount != NULL)
		fail(ERR_PARAMETRES);
    if (gstrcpy(&mount, arg) != 0)
    	fail(-1);
#ifdef DEBUG
	printf("[Function: parse_mount] Mount point set to %s;\n", mount);
#endif

};

// public functions

int parse(int argc, char **argv){
        
    int i = 0;
        
    if ((argc == 2) && ((strcmp(argv[1], OPT_VERSION) == 0) || (strcmp(argv[1], OPT_VERSION) == 0))){
		printf("archfs - filesystem in userspace for rdiff-backup repositories; version %s\n", ARCHFS_VERSION);
		exit(0);	
    };	
    if (argc < 3)
		fail(ERR_PARAMETRES);
    for (i = 1; i < argc; i++){
		if (isOption(argv[i]) == 1)
	    	parse_option(argc, argv, &i);
		else if (mount == NULL)
	    	parse_mount(argv[i]);
		else
	    	parse_repo(argc, argv, &i);
	};
    if (mount == NULL)
		fail(ERR_NO_MOUNT);
    if (repo_count == 0)
		fail(ERR_NO_REPO);
	if ((layout == LAYOUT_LAST) && (structure != STRUCTURE_FULL))
		fail(ERR_FULL_ONLY);
    
    return 0;
    
};
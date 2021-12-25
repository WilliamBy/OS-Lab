#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

/**
 * @brief 打印文件权限
 * 
 * @param status 
 */
void print_st_mode(const struct stat *status)
{
  char buff[10] = {"----------"};
 
  if(status->st_mode & S_IRUSR )
  {
    buff[1] = 'r';
  }
  if(status->st_mode & S_IWUSR )
  {
    buff[2] = 'w';
  }
  if(status->st_mode & S_IXUSR )
  {
    buff[3] = 'x';
  }
  if(status->st_mode & S_IRGRP )
  {
    buff[4] = 'r';
  }
  if(status->st_mode & S_IWGRP )
  {
    buff[5] = 'w';
  }
  if(status->st_mode & S_IXGRP )
  {
    buff[6] = 'x';
  }
  if(status->st_mode & S_IROTH )
  {
    buff[7] = 'r';
  }
  if(status->st_mode & S_IWOTH )
  {
    buff[8] = 'w';
  }
  if(status->st_mode & S_IXOTH )
  {
    buff[9] = 'x';
  }
 
  for(int i = 0; i < 10; i++)
  {
    printf("%c", buff[i]); //循环打印
  }
}

/**
 * @brief 打印文件修改日期
 * 
 * @param status 
 */
void print_st_mtime(const struct stat *status)
{
    printf("%.12s", &(ctime(&(status->st_mtime))[4]));
}

/**
 * @brief 打印文件所属用户、用户组
 * 
 * @param uid 
 * @param gid 
 */
void print_gid_uid(const struct stat *status)
{
  struct passwd *ptr;
  struct group *str; //结构体中存放文件所有者名和文件所有者组名
 
  ptr = getpwuid(status->st_uid); //调用函数获取文件所有者指针
  str = getgrgid(status->st_gid); //调用函数获取文件所有者组指针
  printf("%s %s", ptr->pw_name, str->gr_name); //打印文件所有者和文件所有者组
}

/**
 * @brief 打印文件属性
 * 
 * @param status 
 */
void print_stat(const struct stat *status)
{
    print_st_mode(status);
    printf(" ");
    printf("%ld", status->st_nlink);
    printf(" ");
    print_gid_uid(status);
    printf(" ");
    printf("%ld", status->st_size);
    printf(" ");
    print_st_mtime(status);
}

void printdir(const char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "can't open dir: %s\n", dir);
        return;
    }
    chdir(dir);
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {   //如果是目录，还要递归打印
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            print_stat(&statbuf);
            printf(" %s\n", entry->d_name);
            printdir(entry->d_name, depth + 4);
        }
        else
        {   //如果是文件，直接打印信息
            print_stat(&statbuf);
            printf(" %s\n", entry->d_name);
        }
    }
    chdir("..");
    closedir(dp);
}

int main(int argc, char const *argv[])
{
  const char *default_dir = ".";
  if (argc == 1)
  {
    printdir(default_dir, 0);
  } else {
    printdir(argv[1], 0);
  }
  return 0;
}

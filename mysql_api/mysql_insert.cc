#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>

int main(){
  //创建MySQL操作句柄
  MYSQL* mysql=mysql_init(NULL);
  //连接数据库
  if(mysql_real_connect(mysql,"127.0.0.1","root","","image_system",3306,NULL,0)==NULL){
    //条件为真则数据库连接失败
    perror("mysql_real_connect");
    return 1;
  }
  //防止乱码设置编码格式
  mysql_set_character_set(mysql,"utf8");
  //构建SQL语句
  char sql[4096]={0};
  sprintf(sql,"insert into image_table values(null,'test.png',1024,'2019/08/01','abcdef','png','data/test.png')");
  //执行SQL语句
  int ret=mysql_query(mysql,sql);
  if(ret != 0) {
    printf("执行语句失败! %s\n",mysql_error(mysql));
    return 1;
  }
  //关闭操作句柄
  mysql_close(mysql);
  return 0;
}

#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>

int main() {
  //创建操作句柄
  MYSQL* mysql=mysql_init(NULL);
  //连接数据库
  if(mysql_real_connect(mysql,"127.0.0.1","root","","image_system",3306,NULL,0)==NULL){
    printf("连接失败! %s\n",mysql_error(mysql));
    return 1;
  }
  //设置编码
  mysql_set_character_set(mysql,"utf8");
  //构造SQL语句
  char sql[4096]={0};
  sprintf(sql,"select * from image_table");
  //执行SQL语句
  int ret=mysql_query(mysql,sql);
  if(ret != 0) {
    printf("执行失败! %s\n",mysql_error(mysql));
    return 1;
  }
  //获取查询结果
  MYSQL_RES* res=mysql_store_result(mysql);
  int rows=mysql_num_rows(res);
  int cols=mysql_num_fields(res);
  for(int i=0;i < rows;++i) {
    MYSQL_ROW row=mysql_fetch_row(res);
    for(int j=0;j < cols;++j) {
      printf("%s\t",row[j]);
    }
    printf("\n");
  }
  mysql_free_result(res);
  mysql_close(mysql);
  return 0;
}

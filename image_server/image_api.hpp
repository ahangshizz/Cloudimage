#pragma once

#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>
#include <time.h>
#include <string>
#include <openssl/md5.h>
namespace image_server {
  static MYSQL* MySQLInit() {
    //创建操作句柄
    MYSQL* mysql=mysql_init(NULL);
    //连接数据库
    if(mysql_real_connect(mysql,"127.0.0.1","root","","image_system",3306,NULL,0)==NULL) {
      printf("连接失败! %s\n",mysql_error(mysql));
      return NULL;
    }
    //设置编码格式
    mysql_set_character_set(mysql,"utf8");
    return mysql;
  }
  static void MySQLClose(MYSQL* mysql) {
    mysql_close(mysql);
  }
  class ImageTable {
    public:
      ImageTable(MYSQL* mysql):_mysql(mysql) {}
      bool Insert(const Json::Value& image) {
        char sql[4*1024]={0};
        sprintf(sql,"insert into image_table values(null,'%s',%d,'%s','%s','%s','%s')",image["image_name"].asCString(),               image["size"].asInt(),image["upload_time"].asCString(),
            image["md5"].asCString(),image["type"].asCString(),
            image["path"].asCString());
        printf("[Insert sql] %s\n",sql);

        int ret=mysql_query(_mysql,sql);
        if(ret != 0) {
          printf("插入信息失败! %s\n",mysql_error(_mysql));
          return false;
        }
        return true;
      }

      bool SelectAll(Json::Value* images) {
        char sql[1024*4]={0};
        sprintf(sql,"select * from image_table");
        int ret=mysql_query(_mysql,sql);
        if(ret != 0) {
          printf("执行查找信息失败! %s\n",mysql_error(_mysql));
          return false;
        }
        MYSQL_RES* res=mysql_store_result(_mysql);
        int rows=mysql_num_rows(res);
        for(int i=0;i < rows;++i) {
          MYSQL_ROW row=mysql_fetch_row(res);
          //把查询的信息转换为Json格式
          Json::Value image;
          image["image_id"]=atoi(row[0]);
          image["image_name"]=row[1];
          image["size"]=atoi(row[2]);
          image["upload_time"]=row[3];
          image["md5"]=row[4];
          image["type"]=row[5];
          image["path"]=row[6];
          images->append(image);
        }
        mysql_free_result(res);
        return true;
      }

      bool SelectOne(int image_id,Json::Value* image_ptr) {
        char sql[1024*4]={0};
        sprintf(sql,"select * from image_table where image_id = %d",image_id);
        int ret=mysql_query(_mysql,sql);
        if(ret != 0) {
          printf("SelectOne查询是失败! %s\n",mysql_error(_mysql));
          return false;
        }
        MYSQL_RES* res=mysql_store_result(_mysql);
        int rows=mysql_num_rows(res);
        if(rows != 1) {
          printf("查询到%d条记录\n",rows);
          return false;
        }
        MYSQL_ROW row=mysql_fetch_row(res);
        Json::Value image;
        image["image_id"]=atoi(row[0]);
        image["image_name"]=row[1];
        image["size"]=atoi(row[2]);
        image["upload_time"]=row[3];
        image["md5"]=row[4];
        image["type"]=row[5];
        image["path"]=row[6];
        *image_ptr=image;
        //不释放查询结果会造成内存泄露
        mysql_free_result(res);
        return true;
      }

      bool Delete(int image_id) {
        char sql[1024*4]={0};
        sprintf(sql,"delete from image_table where image_id = %d",image_id);
        int ret=mysql_query(_mysql,sql);
        if(ret != 0) {
          printf("删除信息失败! %s\n",mysql_error(_mysql));
          return false;
        }
        return true;
      }
    private:
      MYSQL* _mysql;
  };
  std::string GetTime() {
    struct tm* t;
    time_t tt;
    time(&tt);
    t=localtime(&tt);
    char buf[100]={0};
    strftime(buf,sizeof(buf),"%Y/%m/%d %H:%M:%s",t);
    return std::string(buf);
  }
  std::string GetMD5(const std::string& str) {
    MD5_CTX md5;
    const char* data=str.c_str();
    unsigned char md[16]={0};
    MD5_Init(&md5);
    MD5_Update(&md5,data,str.size());
    MD5_Final(md,&md5);
    std::string ret;
    char buf[64]={0};
    for(int i=0;i < 16;++i) {
      sprintf(buf,"%x",md[i]);
      ret+=buf;
    }
    ret+='\0';
    return ret;
  }
}

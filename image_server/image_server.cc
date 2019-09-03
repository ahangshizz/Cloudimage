#include <fstream>
#include <signal.h>
#include <sys/stat.h>
#include "httplib.h"
#include "image_api.hpp"

class FileUtil {
  public:
    static bool Write(const std::string& file_name,const std::string& content) {
      std::ofstream file(file_name.c_str());
      if(!file.is_open()) {
        return false;
      }
      file.write(content.c_str(),content.length());
      file.close();
      return true;
    }
    static bool Read(const std::string& file_name,std::string* content) {
      std::ifstream file(file_name.c_str());
      if(!file.is_open()) {
        return false;
      }
      struct stat st;
      stat(file_name.c_str(),&st);
      content->resize(st.st_size);
      //拿到数据大小,读取
      file.read((char*)content->c_str(),content->size());
      file.close();
      return true;
    }
};

MYSQL* mysql=NULL;
int main() {
  using namespace httplib;
  mysql=image_server::MySQLInit();
  image_server::ImageTable image_table(mysql);
  signal(SIGINT,[](int) {
      image_server::MySQLClose(mysql);
      exit(0);
      });
  Server server;
  server.Post("/image",[&image_table](const Request& req,Response& resp){
      Json::FastWriter writer;
      Json::Value resp_json;
      printf("上传图片\n");
      //检验参数
      auto ret=req.has_file("upload");
      if(!ret) {
      printf("上传文件出错!\n");
      resp.status=404;
      //使用Json格式组织返回结果
      resp_json["ok"]=false;
      resp_json["reason"]="上传文件出错,没有upload字段";
      resp.set_content(writer.write(resp_json),"application/json");
      return ;
      }
      //根据文件名获取文件数据file对象
      const auto& file=req.get_file_value("upload");
      const auto& body=req.body.substr(file.offset,file.length);
      Json::Value image;
      image["image_name"]=file.filename;
      image["size"]=(int)file.length;
      image["upload_time"]=image_server::GetTime();
      image["md5"]=image_server::GetMD5(body);
      image["type"]=file.content_type;
      image["path"]="./image/"+file.filename;
      ret=image_table.Insert(image);
      if(!ret) {
        printf("image_table Insert failed!\n");
        resp_json["ok"]=false;
        resp_json["reason"]="数据库插入失败!\n";
        resp.status=500;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
      }
      //把图片保存在指定目录中
      //auto body=req.body.substr(file.offset,file.length);
      FileUtil::Write(image["path"].asString(),body);
      //构造响应数据,上传成功
      resp_json["ok"]=true;
      resp.status=200;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
  });
  server.Get("/image",[&image_table](const Request& req,Response& resp) {
      (void) req;
      printf("获取图片信息\n");
      Json::Value resp_json;
      Json::FastWriter writer;
      //调用接口获取数据
      bool ret=image_table.SelectAll(&resp_json);
      if(!ret) {
      printf("查询数据库失败\n");
      resp_json["ok"]=false;
      resp_json["reason"]="查询数据库失败";
      resp.status=500;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }
      //构造响应结果返回给客户端
      resp.status=200;
      resp.set_content(writer.write(resp_json),"application/json");
      });
  server.Get(R"(/image/(\d+))",[&image_table](const Request& req,Response& resp) {
      Json::FastWriter writer;
      Json::Value resp_json;
      //获取图片id
      int image_id=std::stoi(req.matches[1]);
      printf("获取id为%d的图片信息\n",image_id);
      //查询数据库
      bool ret=image_table.SelectOne(image_id,&resp_json);
      if(!ret) {
      printf("查询数据库失败\n");
      resp_json["ok"]=false;
      resp_json["reason"]="数据库查询失败";
      resp.status=404;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }
      //把查询结果返回给客户端
      resp_json["ok"]=true;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      });
  server.Get(R"(/show/(\d+))",[&image_table](const Request& req,Response& resp) {
      Json::FastWriter writer;
      Json::Value resp_json;
      //根据id在数据库查找
      int image_id=std::stoi(req.matches[1]);
      printf("获取id为%d的图片内容\n",image_id);
      Json::Value image;
      bool ret=image_table.SelectOne(image_id,&image);
      if(!ret) {
      printf("读取数据库失败\n");
      resp_json["ok"]=false;
      resp_json["reason"]="数据库查询失败";
      resp.status=404;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }
      //根据目录找到文件,读取文件内容
      std::string image_body;
      printf("%s\n",image["path"].asCString());
      ret=FileUtil::Read(image["path"].asString(),&image_body);
      if(!ret) {
        printf("读取图片文件失败\n");
        resp_json["ok"]=false;
        resp_json["reason"]="读取图片文件失败";
        resp.status=500;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
      }
      //把文件内容构造成响应
      resp.status=200;
      resp.set_content(image_body,image["type"].asCString());
  });
  server.Delete(R"(/image/(\d+))",[&image_table](const Request& req,Response& resp) {
      Json::FastWriter writer;
      Json::Value resp_json;
      //根据id找到对应的目录
      int image_id=std::stoi(req.matches[1]);
      printf("删除id为%d的图片\n",image_id);
      //查找对应文件路径
      Json::Value image;
      bool ret=image_table.SelectOne(image_id,&image);
      if(!ret) {
      printf("删除图片文件失败\n");
      resp_json["ok"]=false;
      resp_json["reason"]="删除图片文件失败";
      resp.status=404;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }
      //调用数据库进行删除
      ret=image_table.Delete(image_id);
      if(!ret) {
      printf("删除图片文件失败\n");
      resp_json["ok"]=false;
      resp_json["reason"]="删除图片文件失败";
      resp.status=404;
      resp.set_content(writer.write(resp_json),"application/json");
      return;
      }
      //删除文件
      unlink(image["path"].asCString());
      //构造响应
      resp_json["ok"]=true;
      resp.status=200;
      resp.set_content(writer.write(resp_json),"application/json");
  });
  server.set_base_dir("./wwwroot");
  server.listen("0.0.0.0", 8888);
  return 0;
}

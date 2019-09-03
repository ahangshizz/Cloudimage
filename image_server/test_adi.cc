#include "image_api.hpp"

void TestImageTable() {
  Json::StyledWriter writer;
  MYSQL* mysql=image_server::MySQLInit();
  image_server::ImageTable image_table(mysql);
  bool ret=false;
  //插入数据
  //Json::Value image;
  //image["image_name"] = "test.png";
  //image["size"] = 1024;
  //image["upload_time"] = "2019/08/28";
  //image["md5"] = "abcdef";
  //image["type"] = "png";
  //image["path"] = "data/test.png";
  //ret = image_table.Insert(image);
  //printf("ret = %d\n", ret);
  
  //查找所有图片信息
  //Json::Value images;
  //ret=image_table.SelectAll(&images);
  //printf("ret = %d \n",ret);
  //printf("%s\n",writer.write(images).c_str());

  //查找指定图片信息
  //Json::Value image;
  //ret=image_table.SelectOne(1,&image);
  //printf("ret = %d \n",ret);
  //printf("%s\n",writer.write(image).c_str());

  //删除指定图片
  ret=image_table.Delete(1);
  printf("ret = %d\n",ret);
  image_server::MySQLClose(mysql);

}

int main() {
  TestImageTable();
  return 0;
}

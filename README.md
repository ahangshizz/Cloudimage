## 基于HTTP协议的图片处理服务器 Cloudimage
##  项目功能

 - 提供一个存储图片服务器,为图片提供一个唯一的url,用url可以将图片展示在其他网页上

##  项目思路

 - 使用MySQL数据库保存图片的各种信息,并对MySQL提供的接口进行封装
 - 采用md5算法对下载的图片进行校验,保证图片的完整性
 - 采用RESTful风格对HTTP协议进行设计
 - 使用使用JSON数据交换格式组织请求信息与响应信息

##  使用步骤

### 1.向服务器上传图片
首先向服务器请求上传图片的`html`网页,从该网页向服务器上传图片
`47.98.98.123/upload.html`
选择文件进行上传
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190903144427261.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FfaGFuZ19zeno=,size_16,color_FFFFFF,t_70)
### 2.获取图片信息
通过`get`方法使用图片`id`在`/image`目录下获取图片信息
响应消息以`JSON`格式进行返回,返回图片的名称,大小,以及各式等信息
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190903143447240.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FfaGFuZ19zeno=,size_16,color_FFFFFF,t_70)
### 3.获取服务器中的图片内容
使用`Get`方法加上`/show`的后缀以及图片的`id`,可以完整的将图片显示在网页上

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190903143717876.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FfaGFuZ19zeno=,size_16,color_FFFFFF,t_70)
### 4.图片的删除
使用`postman`工具构造`Delete`方法进行删除,根据`/image`后缀以及图片的`id`进行删除,删除数据库中图片的信息以及服务器上图片的内容,删除成功返回`true`

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190903144112250.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FfaGFuZ19zeno=,size_16,color_FFFFFF,t_70)
##  读写实现

将图片内容存储在服务器的磁盘中
```cpp
bool Write(const std::string& file_name,const std::string& content){std::ofstream file(file_name.c_str());
    if (!file.is_open()) {
      return false;
    }
    file.write(content.c_str(), content.length());
    file.close();
    return true;
}
```
获取图片的存储位置,并且拿到请求报文中的`body`字段,将`body`字段内容写入磁盘进行存储

从服务器磁盘读取图片内容通过响应报文返回给客户端
```cpp
bool Read(const std::string& file_name, std::string* content) 
{
    std::ifstream file(file_name.c_str());
    if (!file.is_open()) {
      return false;
    }
    struct stat st;
    stat(file_name.c_str(), &st);
    content->resize(st.st_size);
    file.read((char*)content->c_str(), content->size());
    file.close();
    return true;
 }
```
通过操作系统提供的`stat`进行拿到指定目录文件的大小,通过大小将文件内容全部读取,再通过`httplib`提供的方法将内容返回给客户端

##  项目反思
1.对相同图片而名字不同的图片没有做特殊处理,会浪费一定的服务器磁盘空间
2.没有对url的使用做权限设置,其他人拿到url便可以直接访问图片
3.对于小图片的存储问题没有进行处理,对存储空间有一定的浪费

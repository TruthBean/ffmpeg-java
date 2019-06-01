#FFmpeg-Java

### maven
```xml
<dependency>
    <groupId>com.truthbean</groupId>
    <artifactId>ffmpeg</artifactId>
    <version>0.0.1-RELEASE</version>
</dependency>
```

```
1. 先编写java 代码, 预留native接口

2. 编译成class文件
javac JniTester.java

3. 使用javah 生成native接口对应的c语言头文件
javah -jni JniTester

4. 根据c的头文件实现c代码

5. 编译生成动态链接库文件（推荐使用cmake）
gcc -Wall -fpic -c JniTester.c -I ./ -I /usr/lib/jvm/java-8-openjdk-amd64/include/ -I /usr/lib/jvm/java-8-openjdk-amd64/include/linux/

gcc -Wall -rdynamic -shared -o libjni-tester.so JniTester.o
```

```
install cmake make nasm clang pkg-config

# ubuntu
install gcc

# opensuse 
install gcc-c++
```

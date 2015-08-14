# 设置root logger为INFO级别，使用了ca和fa两个Appender
log4j.rootLogger=INFO, ca, fa
  
#对Appender fa进行设置：
# 这是一个文件类型的Appender，
# 其输出文件（File）为./world.log，
# 输出方式为覆盖方式，
# 输出格式（layout）为PatternLayout
# %m - message
# %n - 回车
# %d - 时间
# %.32c - Logger名称,32代表名称占用32字节
# %-5p - log级别
# %t - thread_id
log4j.appender.fa=org.apache.log4j.FileAppender
log4j.appender.fa.File=./world.log
log4j.appender.fa.Append=false
log4j.appender.fa.layout=org.apache.log4j.PatternLayout
log4j.appender.fa.layout.ConversionPattern=%d [%t] %-5p %.32c - %m%n
  
#对Appender ca进行设置
# 这是一个控制台类型的Appender
#  输出格式（layout）为PatternLayout
# %m - message
# %n - 回车
# %d - 时间
# %.32c - Logger名称,32代表名称占用32字节
# %-5p - log级别
# %t - thread_id
log4j.appender.ca=org.apache.log4j.ConsoleAppender
log4j.appender.ca.layout=org.apache.log4j.PatternLayout
log4j.appender.ca.layout.ConversionPattern=%d [%t] %-5p %.32c - %m%n
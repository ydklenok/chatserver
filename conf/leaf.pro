# 设置root logger为INFO级别，使用了ca和leaf两个Appender  
log4j.rootLogger=DEBUG, leaf
log4j.appender.logfile.encoding=UTF-8

log4j.appender.leaf=org.apache.log4j.DailyRollingFileAppender
log4j.appender.leaf.DatePattern='logs/leaf_'yyyy-MM-dd'.log'
log4j.appender.leaf.ImmediateFlush=true
log4j.appender.leaf.Append=true
log4j.appender.leaf.layout=org.apache.log4j.PatternLayout
log4j.appender.leaf.layout.ConversionPattern=%d [%t] %-5p %.32c(%L) - %m%n
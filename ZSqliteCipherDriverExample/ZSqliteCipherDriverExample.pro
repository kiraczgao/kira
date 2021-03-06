######################################################################
# Automatically generated by qmake (2.01a) Wed Sep 21 15:18:56 2016
######################################################################
QT      += sql
TEMPLATE = app
TARGET = 
DEPENDPATH += . wxsqlite3 zsqlitecipherdriver
INCLUDEPATH += . zsqlitecipherdriver wxsqlite3

# Input
HEADERS += wxsqlite3/codec.h \
           wxsqlite3/rijndael.h \
           wxsqlite3/sha2.h \
           wxsqlite3/sqlite3.h \
           zsqlitecipherdriver/zsqliteciphercachedresult.h \
           zsqlitecipherdriver/zsqlitecipherdriver.h \
           wxsqlite3/sha2.c \
           wxsqlite3/sqlite3.c \
           wxsqlite3/rijndael.c \
           wxsqlite3/codec.c \
           wxsqlite3/codecext.c
SOURCES += main.cpp \
           wxsqlite3/codec.c \
           wxsqlite3/codecext.c \
           wxsqlite3/rijndael.c \
           wxsqlite3/sha2.c \
           wxsqlite3/sqlite3.c \
           wxsqlite3/sqlite3secure.c \
           zsqlitecipherdriver/zsqliteciphercachedresult.cpp \
           zsqlitecipherdriver/zsqlitecipherdriver.cpp

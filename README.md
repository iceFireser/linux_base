# linux_base
linux基础学习

 clickhouse 下载代码方法：
git clone git@github.com:ClickHouse/ClickHouse.git  : 注意不要递归下载
cd <clickhouse目录>
sed -i 's/https:\/\/github.com\//git@github.com:/' .gitmodules : 修改submodule为ssh方式
cd ..
git submodule sync : 同步修改
git submodule update --init : 注意不要递归下载
find . -name ".gitsubmodules" | xargs -i sed 's/https:\/\/github.com\//git@github.com:/' -i {} ：修改submodule的submodule为ssh方式
git submodule update --init --recursive ： 递归下载（submodule目前只嵌套了二层）

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListView>
#include <QtWidgets/QTreeView>
#include <queue>
#include <thread>
#include <mutex>
#include <iostream>
#include "cryptopp/default.h"
#include "cryptopp/files.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
class DecryptThread
{
public:
    DecryptThread(){m_finish = false;}
    ~DecryptThread() = default;
    void setFileNames(const std::vector<std::string>& fileNames,const std::string& updir){
        upDir = updir;
        while (!fileNameQueue.empty())
            fileNameQueue.pop();
        for(const auto& path : fileNames)
            fileNameQueue.push(path);
    }
    void Start(){
        m_Thread = std::thread(&DecryptThread::run,this);
    }
    void Stop(){
        if(m_Thread.joinable()){
            m_Thread.join();
        }
    }
    inline void setPassword(const std::string& pass){m_Password = pass;}
private:
    std::thread m_Thread;
    std::vector<std::thread> threadPool;
    std::queue<std::string> fileNameQueue;
    std::string upDir;
    std::mutex m_Mutex;
    std::atomic<bool> m_finish;
    std::string m_Password;

    void run(){
        m_finish = false;
        uint nOfThread = fileNameQueue.size();
        if(fileNameQueue.size()>16){
            nOfThread = 16;
        }
        const size_t sizeOfQueue = fileNameQueue.size();
        for(uint i=0;i<nOfThread;i++){
            threadPool.push_back(std::thread([this,sizeOfQueue](){
                while(true){                    
                    std::string path;
                    {
                        std::unique_lock<std::mutex> lck(m_Mutex);
                        if(!fileNameQueue.empty()){
                            std::cout<<double(sizeOfQueue)<<std::endl;
                            std::cout<<double(fileNameQueue.size())<<std::endl;
                            std::cout<<((double(sizeOfQueue - fileNameQueue.size())*100.0)/double(sizeOfQueue))<<std::endl;
                            path = fileNameQueue.front();                        
                            fileNameQueue.pop();
                        }else{
                            break;
                        }
                    }
                    //std::cout<<std::this_thread::get_id()<<" : "<<path<<std::endl;

                    fs::path filePath(path);
                    std::cout<<filePath.extension()<<std::endl;
                    std::cout<<path<<std::endl;
                    size_t i = path.rfind('.enc', path.length());
                    if (i != std::string::npos) {
                        path = path.substr(0, i-3);
                    }
                    std::cout<<path<<std::endl;
                    std::cout<<m_Password<<std::endl;
                    CryptoPP::FileSource f(filePath.string().c_str(), true, new CryptoPP::DefaultDecryptor(m_Password.c_str(),new CryptoPP::FileSink(path.c_str()),true));                    
                    fs::remove(filePath.string().c_str());
                }
            }));
        }
        for(auto& thread : threadPool){
            if(thread.joinable()){
                thread.join();
            }
        }
        m_finish = true;
        std::cout<<"Done"<<std::endl;
    }
};
class EncryptThread
{
public:
    EncryptThread(){m_finish = false;}
    ~EncryptThread() = default;
    void setFileNames(const std::vector<std::string>& fileNames,const std::string& updir){
        upDir = updir;
        while (!fileNameQueue.empty())
            fileNameQueue.pop();
        for(const auto& path : fileNames)
            fileNameQueue.push(path);
    }
    void Start(){
        m_Thread = std::thread(&EncryptThread::run,this);
    }
    void Stop(){
        if(m_Thread.joinable()){
            m_Thread.join();
        }
    }
    inline void setPassword(const std::string& pass){m_Password = pass;}
private:
    std::thread m_Thread;
    std::vector<std::thread> threadPool;
    std::queue<std::string> fileNameQueue;
    std::string upDir;
    std::mutex m_Mutex;
    std::atomic<bool> m_finish;
    std::string m_Password;  

    void run(){
        m_finish = false;
        uint nOfThread = fileNameQueue.size();
        if(fileNameQueue.size()>16){
            nOfThread = 16;
        }
        for(uint i=0;i<nOfThread;i++){
            threadPool.push_back(std::thread([this](){
                while(true){
                    std::string path;
                    {
                        std::unique_lock<std::mutex> lck(m_Mutex);
                        if(!fileNameQueue.empty()){
                            path = fileNameQueue.front();                        
                            fileNameQueue.pop();
                        }else{
                            break;
                        }
                    }
                    //std::cout<<std::this_thread::get_id()<<" : "<<path<<std::endl;
                    std::cout<<m_Password<<std::endl;
                    CryptoPP::FileSource f(path.c_str(), true, new CryptoPP::DefaultEncryptor(m_Password.c_str(),new CryptoPP::FileSink((path+std::string(".enc")).c_str())),true);
                    fs::remove(path.c_str());
                }
            }));
        }
        for(auto& thread : threadPool){
            if(thread.joinable()){
                thread.join();
            }
        }
        m_finish = true;
        std::cout<<"Done"<<std::endl;
    }
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
private:
    
    std::vector<std::string> m_EnFilesPath;
    std::string m_EnUpDir;
    std::vector<std::string> m_DeFilesPath;
    std::string m_DeUpDir;
private:
    
};
class SFDFileDialog : public QFileDialog
{
Q_OBJECT
public:
SFDFileDialog(QWidget * parent = 0, const QString & caption = QString(),
const QString & directory = QString(),
const QString & filter = QString() );
~SFDFileDialog();

public slots:
void accept ();
};
#endif // MAINWINDOW_H


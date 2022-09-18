#include "mainwindow.h"
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListView>
#include <QtWidgets/QTreeView>
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
SFDFileDialog::SFDFileDialog(QWidget *parent,
                         const QString &caption,
                         const QString &directory,
                         const QString &filter)
: QFileDialog(parent, caption, directory, filter)
{
    // Try to select multiple files and directories at the same time in QFileDialog
    this->setFileMode(QFileDialog::ExistingFiles);
    this->setOption(QFileDialog::DontUseNativeDialog, true);
    QListView *l = this->findChild<QListView*>("listView");
    if (l) {
        l->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    QTreeView *t = this->findChild<QTreeView*>();
    if (t) {
        t->setSelectionMode(QAbstractItemView::MultiSelection);
    }

    // std::cout<<"C"<<std::endl;
    // QList<QPushButton*> ls = this->findChildren<QPushButton*>();
    // QPushButton* openBT;
    // for(uint i=0;i<ls.size();i++){
    //     std::cout<<ls[i]->text().toStdString()<<std::endl;
    //     auto s = ls[i]->text().toStdString();
    //     if(s.find("Choose")<s.length()){
    //         openBT=ls[i];
    //         std::cout<<openBT->text().toStdString()<<std::endl;
    //         openBT->disconnect();
    //         connect(openBT,&QPushButton::clicked,[this](){
    //             QStringList _fnames = this->selectedFiles();
    //             for (int i = 0; i < _fnames.size(); ++i)
    //                 std::cout << _fnames.at(i).toLocal8Bit().constData() << std::endl;
    //         });
    //     }
    // }

    // // Try to select multiple files and directories at the same time in QFileDialog
    
}

SFDFileDialog::~SFDFileDialog()
{
}

void SFDFileDialog::accept()
{
    QStringList files = selectedFiles();
    if (files.isEmpty())
    return;
    emit filesSelected(files);
    QDialog::accept();
}
inline std::string getUpDir(const std::string& filePath){
			size_t i = filePath.rfind('/', filePath.length());
			if (i != std::string::npos) {
				return(filePath.substr(0, i));
			}
			return("");
		}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("MainWindow"));
    this->resize(700, 100);
    QWidget * centralwidget = new QWidget(this);
    QVBoxLayout* centralWidget_VerticalLayout = new QVBoxLayout(centralwidget);
    QGroupBox* encrypt_GB = new QGroupBox("Encrypt Files");
    centralWidget_VerticalLayout->addWidget(encrypt_GB);
    QGridLayout* encrypt_GB_GL = new QGridLayout(encrypt_GB);
    QPushButton* select_enc_file_PB = new QPushButton("...",encrypt_GB);
    QLineEdit* password_LE = new QLineEdit(encrypt_GB);
    QLineEdit* password_c_LE = new QLineEdit(encrypt_GB);
    QPushButton* showHide_PB = new QPushButton("Show",encrypt_GB);
    showHide_PB->setCheckable(true);
    password_LE->setEchoMode(QLineEdit::Password);
    password_c_LE->setEchoMode(QLineEdit::Password);
    encrypt_GB_GL->addWidget(new QLabel("Select Files",encrypt_GB),0,0,1,5);
    encrypt_GB_GL->addWidget(select_enc_file_PB,0,5,1,5);
    encrypt_GB_GL->addWidget(new QLabel("Password"),1,0,1,2);
    encrypt_GB_GL->addWidget(password_LE,1,2,1,2);
    encrypt_GB_GL->addWidget(new QLabel("Confirm"),1,4,1,2);
    encrypt_GB_GL->addWidget(password_c_LE,1,6,1,2);
    encrypt_GB_GL->addWidget(showHide_PB,1,8,1,2);
    QLabel* passwordMatch_L = new QLabel(encrypt_GB);
    encrypt_GB_GL->addWidget(passwordMatch_L,2,0,1,8);
    QPushButton* encrypt_PB = new QPushButton("Encrypt",encrypt_GB);
    encrypt_GB_GL->addWidget(encrypt_PB,3,8,1,2);
    encrypt_PB->setEnabled(false);

    connect(select_enc_file_PB,&QPushButton::clicked,[=](){
        SFDFileDialog *_f_dlg = new SFDFileDialog();        
        int nMode = _f_dlg->exec();
        QStringList _fnames = _f_dlg->selectedFiles();
        if(_fnames.size()>0){
            std::string selectedFiles = _fnames.at(0).toLocal8Bit().constData();
            size_t i = selectedFiles.rfind('/', selectedFiles.length());
            if (i != std::string::npos) {
				m_EnUpDir = selectedFiles.substr(0, i);
			}
        }
        for (int i = 0; i < _fnames.size(); ++i){
            std::string selectedFiles = _fnames.at(i).toLocal8Bit().constData();
            //std::cout << _fnames.at(i).toLocal8Bit().constData() << std::endl;
            if(fs::is_directory(selectedFiles)){
                for (const auto& dirEntry : fs::recursive_directory_iterator(selectedFiles)){
                    if(!(fs::is_directory(dirEntry))){
                        m_EnFilesPath.push_back(dirEntry.path());
                    }
                }
            }else{                
                m_EnFilesPath.push_back(selectedFiles);
            }
        }

        // for(const auto& path : m_EnFilesPath){
        //     std::cout<<path<<std::endl;
        // }
        //Check directory or not and find all files under dirs.

        //find each 
        delete _f_dlg;
    });
    connect(showHide_PB,QOverload<bool>::of(&QPushButton::toggled),[=](bool state){
        if(state){
            showHide_PB->setText("Hide");
            password_LE->setEchoMode(QLineEdit::Normal);
            password_c_LE->setEchoMode(QLineEdit::Normal);
        }else{
            showHide_PB->setText("Show");
            password_LE->setEchoMode(QLineEdit::Password);
            password_c_LE->setEchoMode(QLineEdit::Password);
        }
    });
    connect(password_LE,QOverload<const QString&>::of(&QLineEdit::textEdited),[=](const QString& text){
        if(text!=password_c_LE->text()){
            passwordMatch_L->setText("Passwords are not matched!");
            encrypt_PB->setEnabled(false);
        }else{
            if((password_LE->text().length() > 0)&&(password_c_LE->text().length() > 0)){
                passwordMatch_L->setText("Passwords are matched");                
                encrypt_PB->setEnabled(true);
            }else{
                passwordMatch_L->setText("Passwords lenght should be bigger than 0");
            }
        }
    });
    connect(password_c_LE,QOverload<const QString&>::of(&QLineEdit::textEdited),[=](const QString& text){
        if(text!=password_LE->text()){
            passwordMatch_L->setText("Passwords are not matched!");
            encrypt_PB->setEnabled(false);
        }else{
            if((password_LE->text().length() > 0)&&(password_c_LE->text().length() > 0)){
                passwordMatch_L->setText("Passwords are matched");                
                encrypt_PB->setEnabled(true);
            }else{
                passwordMatch_L->setText("Passwords lenght should be bigger than 0");
            }
        }
    });

    connect(encrypt_PB,&QPushButton::clicked,[=](){
        encrypt_PB->setEnabled(false);
        EncryptThread encr;
        encr.setFileNames(m_EnFilesPath,m_EnUpDir);
        encr.setPassword(password_LE->text().toStdString());
        encr.Start();
        encr.Stop();
        encrypt_PB->setEnabled(true);
        
    });
    QGroupBox* decrypt_GB = new QGroupBox("Decrypt Files");
    centralWidget_VerticalLayout->addWidget(decrypt_GB);
    QGridLayout* decrypt_GB_GL = new QGridLayout(decrypt_GB);
    QPushButton* select_dec_file_PB = new QPushButton("...",decrypt_GB);
    QLineEdit* Dpassword_LE = new QLineEdit(decrypt_GB);
    QLineEdit* Dpassword_c_LE = new QLineEdit(decrypt_GB);
    QPushButton* DshowHide_PB = new QPushButton("Show",decrypt_GB);
    DshowHide_PB->setCheckable(true);
    Dpassword_LE->setEchoMode(QLineEdit::Password);
    Dpassword_c_LE->setEchoMode(QLineEdit::Password);
    decrypt_GB_GL->addWidget(new QLabel("Select Files",encrypt_GB),0,0,1,5);
    decrypt_GB_GL->addWidget(select_dec_file_PB,0,5,1,5);
    decrypt_GB_GL->addWidget(new QLabel("Password"),1,0,1,2);
    decrypt_GB_GL->addWidget(Dpassword_LE,1,2,1,2);
    decrypt_GB_GL->addWidget(new QLabel("Confirm"),1,4,1,2);
    decrypt_GB_GL->addWidget(Dpassword_c_LE,1,6,1,2);
    decrypt_GB_GL->addWidget(DshowHide_PB,1,8,1,2);
    QLabel* DpasswordMatch_L = new QLabel(decrypt_GB);
    decrypt_GB_GL->addWidget(DpasswordMatch_L,2,0,1,8);
    QPushButton* decrypt_PB = new QPushButton("Decrypt",encrypt_GB);
    decrypt_GB_GL->addWidget(decrypt_PB,3,8,1,2);
    decrypt_PB->setEnabled(false);

    connect(select_dec_file_PB,&QPushButton::clicked,[=](){
        SFDFileDialog *_f_dlg = new SFDFileDialog();        
        int nMode = _f_dlg->exec();
        QStringList _fnames = _f_dlg->selectedFiles();
        if(_fnames.size()>0){
            std::string selectedFiles = _fnames.at(0).toLocal8Bit().constData();
            size_t i = selectedFiles.rfind('/', selectedFiles.length());
            if (i != std::string::npos) {
				m_DeUpDir = selectedFiles.substr(0, i);
			}
        }
        for (int i = 0; i < _fnames.size(); ++i){
            std::string selectedFiles = _fnames.at(i).toLocal8Bit().constData();
            //std::cout << _fnames.at(i).toLocal8Bit().constData() << std::endl;
            if(fs::is_directory(selectedFiles)){
                for (const auto& dirEntry : fs::recursive_directory_iterator(selectedFiles)){
                    if(!(fs::is_directory(dirEntry))){
                        m_DeFilesPath.push_back(dirEntry.path());
                    }
                }
            }else{                
                m_DeFilesPath.push_back(selectedFiles);
            }
        }

        // for(const auto& path : m_EnFilesPath){
        //     std::cout<<path<<std::endl;
        // }
        //Check directory or not and find all files under dirs.

        //find each 
        delete _f_dlg;
    });
    connect(DshowHide_PB,QOverload<bool>::of(&QPushButton::toggled),[=](bool state){
        if(state){
            DshowHide_PB->setText("Hide");
            Dpassword_LE->setEchoMode(QLineEdit::Normal);
            Dpassword_c_LE->setEchoMode(QLineEdit::Normal);
        }else{
            DshowHide_PB->setText("Show");
            Dpassword_LE->setEchoMode(QLineEdit::Password);
            Dpassword_c_LE->setEchoMode(QLineEdit::Password);
        }
    });
    connect(Dpassword_LE,QOverload<const QString&>::of(&QLineEdit::textEdited),[=](const QString& text){        
        if(text!=Dpassword_c_LE->text()){
            DpasswordMatch_L->setText("Passwords are not matched!");
            decrypt_PB->setEnabled(false);
        }else{
            if((Dpassword_LE->text().length() > 0)&&(Dpassword_c_LE->text().length() > 0)){
                DpasswordMatch_L->setText("Passwords are matched");    
                decrypt_PB->setEnabled(true);
            }else{
                DpasswordMatch_L->setText("Passwords lenght should be bigger than 0");
            }
        }
    });
    connect(Dpassword_c_LE,QOverload<const QString&>::of(&QLineEdit::textEdited),[=](const QString& text){
        if(text!=Dpassword_LE->text()){
            DpasswordMatch_L->setText("Passwords are not matched!");
            decrypt_PB->setEnabled(false);
        }else{
            if((Dpassword_LE->text().length() > 0)&&(Dpassword_c_LE->text().length() > 0)){
                DpasswordMatch_L->setText("Passwords are matched");    
                decrypt_PB->setEnabled(true);
            }else{
                DpasswordMatch_L->setText("Passwords lenght should be bigger than 0");
            }
        }
    });

    connect(decrypt_PB,&QPushButton::clicked,[=](){
        DecryptThread decr;
        decr.setFileNames(m_DeFilesPath,m_DeUpDir);
        decr.setPassword(Dpassword_LE->text().toStdString());
        decr.Start();
        decr.Stop();
        
    });
    
    
    this->setCentralWidget(centralwidget);
    this->setWindowTitle("FileYZ");
}

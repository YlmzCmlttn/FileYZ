
#include "cryptopp/default.h"
#include "cryptopp/files.h"
#include "mainwindow.h"

#include <QApplication>
using namespace CryptoPP;
void EncryptFile(const char *fin, const char *fout, const char
*passwd)
{
    try{        
        FileSource f(fin, true, new DefaultEncryptor(passwd,new FileSink(fout)),true);
    }
    catch(const CryptoPP::FileStore::OpenErr& e)
    {
        std::cerr << e.what() << " Open Catched \n";        
    }
    catch(const CryptoPP::FileStore::ReadErr& e)
    {
        std::cerr << e.what() << " Read Catched \n";        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void DecryptFile(const char *fin, const char *fout, const char
*passwd)
{   
    try
    {
        FileSource f(fin, true, new DefaultDecryptor(passwd, new FileSink(fout)),true);
    }
    catch(const CryptoPP::KeyBadErr& e)
    {
        std::cerr << e.what() << " Catched \n";
        return;        
    }
    catch(const CryptoPP::FileStore::OpenErr& e)
    {
        std::cerr << e.what() << " Open Catched \n";        
        return;
    }
    catch(const CryptoPP::FileStore::ReadErr& e)
    {
        std::cerr << e.what() << " Read Catched \n";        
        return;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
    
}

int main(int argc, char *argv[]){
    // EncryptFile("/home/cemo/TestProjects/build/A.docx","/home/cemo/TestProjects/build/A2.docx.enc","123456789");
    // DecryptFile("/home/cemo/TestProjects/build/A.pdf.enc","/home/cemo/TestProjects/build/A.pdf","123456");

    QApplication a(argc, argv);
    MainWindow w;
    //system("gsettings set org.gnome.shell.extensions.dash-to-dock click-action 'cycle-windows'");
    w.show();
    //system("bash ./test.sh");
    //system("echo sanane | sudo -S gsettings set org.gnome.shell.extensions.dash-to-dock click-action 'cycle-windows'");
    return a.exec();
}
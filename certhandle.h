#ifndef CERTHANDLE_H
#define CERTHANDLE_H

#include <QObject>

#include <stdio.h>
#include <iostream>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/objects.h>
#include <openssl/ocsp.h>

class CertHandle : public QObject
{
    Q_OBJECT
public:
    explicit CertHandle(QObject *parent = nullptr);

private:

    int _do_X509_sign(X509 *cert, EVP_PKEY *pkey, const EVP_MD *md);
    char _file_name_pub[80];    //location of public key
    char _file_name_key[80];    //location of private key
    char _file_name_csr[80];    //location of csr
    char _file_name[80];        //location used as a genral file

    EVP_PKEY *_pKey = NULL;
    X509 * _ca = NULL;
    X509_REQ* _drone_req = NULL;

signals:
    void cert_load_success();
    void csr_load_success();
    void key_generate_success();
    void key_load_success();

public slots:
    void generate_keys(QString folder);
    void generate_cert(QString folder);
    void load_cert(QString file);
    void load_csr(QString file);
    void load_keys(QString file);
    void convert_der_to_pem(QString file);
};

#endif // CERTHANDLE_H

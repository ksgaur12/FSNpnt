#include "certhandle.h"
#include <QDebug>

CertHandle::CertHandle(QObject *parent) : QObject(parent)
{

}

void CertHandle::generate_keys(QString folder)
{
    int     ret = 0;
    RSA     *r = NULL;
    BIGNUM  *bne = NULL;
    BIO     *bp_public = NULL, *bp_private = NULL;

    int             nVersion = 1;
    int     bits = 2048;
    unsigned long e = RSA_F4;

    X509_REQ        *x509_req = NULL;
    X509_NAME       *x509_name = NULL;
    EVP_PKEY        *pKey = NULL;
    RSA             *tem = NULL;
    BIO             *out = NULL, *bio_err = NULL;

    const char      *szCountry = "IN";
    const char      *szProvince = "DL";
    const char      *szCity = "Delhi";
    const char      *szOrganization = "fs pvt. ltd.";
    const char      *szCommon = "fs";
    //    const char      *szPath = "x509Req.pem";

    folder.remove(0, 5);//remove the file:// appeneded to the folder
    QString file;
    file = folder + "/public.pem";
    memcpy( _file_name_pub, file.toStdString().c_str() ,file.size());

    file = folder + "/private_key.pem";
    memcpy(_file_name_key, file.toStdString().c_str(), file.size());

    file = folder + "/csr_req.pem";
    memcpy(_file_name_csr, file.toStdString().c_str(), file.size());

    bne = BN_new();
    ret = BN_set_word(bne, e);

    if(ret != 1){
        goto free_all;
    }

    r = RSA_new();
    ret = RSA_generate_key_ex(r, bits, bne, NULL);
    if(ret != 1){
        goto free_all;
    }

    // 2. save public key
    bp_public = BIO_new_file(_file_name_pub, "w+");
    ret = PEM_write_bio_RSAPublicKey(bp_public, r);
    if(ret != 1){
        goto free_all;
    }

    // 3. save private key
    bp_private = BIO_new_file(_file_name_key, "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

    // 4 set version of x509 req
    x509_req = X509_REQ_new();
    ret = X509_REQ_set_version(x509_req, nVersion);

    if(ret !=1){
        goto free_all;
    }

    // 5. set subject of x509 req
    x509_name = X509_REQ_get_subject_name(x509_req);

    ret = X509_NAME_add_entry_by_txt(x509_name,"C", MBSTRING_ASC, (const unsigned char*)szCountry, -1, -1, 0);
    if (ret != 1){
        goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"ST", MBSTRING_ASC, (const unsigned char*)szProvince, -1, -1, 0);
    if (ret != 1){
        goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"L", MBSTRING_ASC, (const unsigned char*)szCity, -1, -1, 0);
    if (ret != 1){
        goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"O", MBSTRING_ASC, (const unsigned char*)szOrganization, -1, -1, 0);
    if (ret != 1){
        goto free_all;
    }

    ret = X509_NAME_add_entry_by_txt(x509_name,"CN", MBSTRING_ASC, (const unsigned char*)szCommon, -1, -1, 0);
    if (ret != 1){
        goto free_all;
    }

    // 6. set public key of x509 req
    pKey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pKey, r);
    r = NULL;  // will be free rsa when EVP_PKEY_free(pKey)

    ret = X509_REQ_set_pubkey(x509_req, pKey);
    if (ret != 1){
        goto free_all;
    }

    // 7. set sign key of x509 req
    ret = X509_REQ_sign(x509_req, pKey, EVP_sha256());
    if(ret <= 0){
        goto free_all;
    }

    out = BIO_new_file(_file_name_csr, "w+");
    ret = PEM_write_bio_X509_REQ(out, x509_req);


    memset(_file_name_pub, 0, sizeof _file_name_pub);
    memset(_file_name_key, 0, sizeof _file_name_key);
    memset(_file_name_csr, 0, sizeof _file_name_csr);
free_all:
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    BIO_free_all(out);
    X509_REQ_free(x509_req);
    EVP_PKEY_free(pKey);
    RSA_free(r);
    BN_free(bne);

    emit key_generate_success();
}

int CertHandle::_do_X509_sign(X509 *cert, EVP_PKEY *pkey, const EVP_MD *md)
{
    int rv;
    EVP_MD_CTX mctx;
    EVP_PKEY_CTX *pkctx = NULL;

    EVP_MD_CTX_init(&mctx);
    rv = EVP_DigestSignInit(&mctx, &pkctx, md, NULL, pkey);

    if(rv > 0)
        rv = X509_sign_ctx(cert, &mctx);
    EVP_MD_CTX_cleanup(&mctx);
    return rv > 0 ? 1 : 0;
}

void CertHandle::generate_cert(QString folder)
{
    folder.remove(0, 5);//remove the file:// appeneded to the folder
    QString file;
    file = folder + "/drone_cert.pem";
    memcpy( _file_name, file.toStdString().c_str() ,file.size());

    int ret = 0;
    int serial = 1;
    long days = 365 * 3; // 3 years
    char *md = NULL;

    X509 * ca = NULL;
    EVP_PKEY *pkey = NULL, *pktmp = NULL;

    X509_NAME *subject = NULL, *tmpname = NULL;
    X509 * cert = NULL;
    BIO *out = NULL;

    cert = X509_new();

    //set verion to X509 certificate
    if(!X509_set_version(cert, 2))
        goto free_all;

    //set serial
    ASN1_INTEGER_set(X509_get_serialNumber(cert), serial);

    //set issuer name from ca
    if(!X509_set_issuer_name(cert, X509_get_subject_name(_ca)))
        goto free_all;

    //set time
    X509_gmtime_adj(X509_get_notBefore(cert),0);
    X509_gmtime_adj(X509_get_notAfter(cert), days);

    //set subject from req
    tmpname = X509_REQ_get_subject_name(_drone_req);
    subject = X509_NAME_dup(tmpname);
    if(!X509_set_subject_name(cert, subject))
        goto free_all;

    //set pubkey from req
    pktmp = X509_REQ_get_pubkey(_drone_req);
    ret = X509_set_pubkey(cert, pktmp);
    EVP_PKEY_free(pktmp);
    if(!ret)
        goto free_all;

    if(!_do_X509_sign(cert, _pKey, EVP_sha256()))
        goto free_all;

    out = BIO_new_file(_file_name, "w+");
    ret = PEM_write_bio_X509(out, cert);

    free_all:
        X509_free(cert);
        BIO_free_all(out);

        X509_REQ_free(_drone_req);
        X509_free(_ca);
        EVP_PKEY_free(pkey);

    memset(_file_name, 0, sizeof _file_name);
}

void CertHandle::load_keys(QString file)
{
    qDebug() << file;
    file.remove(0, 5); //remove the file prefix
    memcpy(_file_name_key, file.toStdString().c_str(), file.size());

    bool ret;
    BIO *in = NULL;
    RSA *r = NULL;

    in = BIO_new_file(_file_name_key, "r");
    ret = (PEM_read_bio_RSAPrivateKey(in, &r, NULL, NULL) != NULL);
    if(!ret)
        goto free_;

    _pKey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(_pKey, r);
    r = NULL;

    memset(_file_name_key, 0, sizeof _file_name_key);

    free_:
        BIO_free(in);

    emit key_load_success();
}

void CertHandle::load_cert(QString file)
{
    qDebug() << file;

    file.remove(0, 5); //remove the file prefix
    memcpy(_file_name, file.toStdString().c_str(), file.size());

    bool ret;
    BIO *in = NULL;

    in = BIO_new_file(_file_name, "r");
    ret = (PEM_read_bio_X509(in, &_ca, NULL, NULL) != NULL);

    memset(_file_name, 0, sizeof _file_name);
    BIO_free(in);

    emit cert_load_success();
}

void CertHandle::load_csr(QString file)
{
    file.remove(0, 5); //remove the file prefix
    memcpy(_file_name, file.toStdString().c_str(), file.size());

    bool ret;
    BIO *in = NULL;

    in = BIO_new_file(_file_name, "r");
    ret = (PEM_read_bio_X509_REQ(in, &_drone_req, NULL, NULL) != NULL);
    memset(_file_name, 0, sizeof _file_name);

    free_:
        BIO_free(in);

        emit csr_load_success();
}

#include <db_cxx.h>
#include "db_base.h"
#include "pki_key.h"
#include <qstringlist.h>

#ifndef DB_KEY_H
#define DB_KEY_H


class db_key: public db_base
{
    public:
	db_key(DbEnv *dbe, string DBfile, string DB, QListBox *l, pki_key *tg);
	QStringList getPrivateDesc();
};

#endif
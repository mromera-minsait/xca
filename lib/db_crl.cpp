/*
 * Copyright (C) 2001 Christian Hohnstaedt.
 *
 *  All rights reserved.
 *
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the name of the author nor the names of its contributors may be 
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * This program links to software with different licenses from:
 *
 *	http://www.openssl.org which includes cryptographic software
 * 	written by Eric Young (eay@cryptsoft.com)"
 *
 *	http://www.sleepycat.com
 *
 *	http://www.trolltech.com
 * 
 *
 *
 * http://www.hohnstaedt.de/xca
 * email: christian@hohnstaedt.de
 *
 * $Id$
 *
 */                           


#include "db_crl.h"


db_crl::db_crl(DbEnv *dbe, string DBfile, QListView *l, DbTxn *tid, db_x509 *cts)
	:db_base(dbe, DBfile, "crldb", tid)
{
	listView = l;
	loadContainer();
	crlicon = loadImg("crl.png");
	listView->addColumn(tr("Issuer"));
	listView->addColumn(tr("Count"));
	certs = cts;
	updateView();
}

pki_base *db_crl::newPKI(){
	return new pki_crl();
}



void db_crl::updateViewPKI(pki_base *pki)
{
        db_base::updateViewPKI(pki);
        if (! pki) return;
        QListViewItem *current = (QListViewItem *)pki->getPointer();
        if (!current) return;
	current->setPixmap(0, *crlicon);
	current->setText(1, ((pki_crl *)pki)->issuerName().c_str());
	current->setText(2, QString::number(((pki_crl *)pki)->numRev()));
	
}

void db_crl::preprocess()
{
	return;
	CERR("preprocess CRL");
	if ( container.isEmpty() ) return ;
	QListIterator<pki_base> iter(container); 
	for ( ; iter.current(); ++iter ) { // find the signer and the key of the certificate...
	MARK
		revokeCerts((pki_crl *)iter.current());
	MARK
	}
}	

void db_crl::revokeCerts(pki_crl *crl)
{
	int numc, i;
	pki_x509 *rev, *iss;
	
	MARK
	iss = certs->getBySubject(crl->getIssuerX509_NAME());
	MARK
	numc = crl->numRev();
	MARK
	for (i=0; i<numc; i++) {
		CERR("SERIAL: "<<  crl->getSerial(i));
		rev = certs->getByIssSerial(iss, crl->getSerial(i));
		if (rev != NULL) {
			rev->setRevoked(crl->getRevDate(i));
		}
	}
}

void db_crl::inToCont(pki_base *pki)
{
	revokeCerts((pki_crl *)pki);
	container.append(pki);
}


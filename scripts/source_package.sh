#!/bin/sh

vers=20090329
tag=s20090329

git archive --format=tar --prefix=camctl-$vers/ $tag | gzip -9 > camctl-$vers.tar.gz

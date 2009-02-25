#!/bin/sh

vers=20090224
tag=s20090224

git archive --format=tar --prefix=camctl-$vers/ $tag | gzip -9 > camctl-$vers.tar.gz

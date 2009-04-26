#!/bin/sh

vers=20090425
tag=s20090425

git archive --format=tar --prefix=camctl-$vers/ $tag | gzip -9 > camctl-$vers.tar.gz

#!/bin/sh

vers=20090301
tag=s20090301

git archive --format=tar --prefix=camctl-$vers/ $tag | gzip -9 > camctl-$vers.tar.gz

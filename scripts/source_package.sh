#!/bin/sh

vers=20100408
tag=s20100408

git archive --format=tar --prefix=camctl-$vers/ $tag | gzip -9 > camctl-$vers.tar.gz

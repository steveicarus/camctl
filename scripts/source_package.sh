#!/bin/sh

vers=20090309
tag=s20090309

git archive --format=tar --prefix=camctl-$vers/ $tag | gzip -9 > camctl-$vers.tar.gz

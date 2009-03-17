#!/bin/sh

vers=20090316
tag=s20090316

git archive --format=tar --prefix=camctl-$vers/ $tag | gzip -9 > camctl-$vers.tar.gz

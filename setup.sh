#!/bin/bash
#
# Copyright 2016 The IskipTests Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################################

function checkout_and_clone()
{
	app=${1}
	url=${2}
	target_commit=${3}


	mkdir -p ${tp}
	target_dir=${tp}/${app}

	# clone (if needed)
	if [ -d ${target_dir}/.git ] ; then
		(cd ${target_dir} && git fetch)
	else
		git clone ${url} ${target_dir}
	fi

	# checkout the right version
	(cd ${target_dir} && git checkout ${target_commit})

	if [ -d patches/${app} ] ; then
		# apply patches
		for p in patches/${app}/*.patch; do
			pp=$(realpath "${p}")
			(cd ${target_dir} && patch -p1 < ${pp})
		done
	fi
}

tp=third_party
test -d ${tp} && { echo "please remove the ${tp} directory"; exit -1; }

checkout_and_clone cryptoc https://chromium.googlesource.com/chromiumos/third_party/cryptoc 5319e83
checkout_and_clone mini-printf https://github.com/mludvig/mini-printf.git b575924
checkout_and_clone tiny-AES128-C https://github.com/kokke/tiny-AES128-C.git f832b41

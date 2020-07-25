#!/bin/bash -e
#
#    Copyright 2020 NNN1590
#
#    This file is part of G'MIC-Qt, a generic plug-in for raster graphics
#    editors, offering hundreds of filters thanks to the underlying G'MIC
#    image processing framework.
#
#    gmic_qt is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    gmic_qt is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with gmic_qt.  If not, see <http://www.gnu.org/licenses/>.
#

cd "$(dirname "${0}")"

function main() {
	set -e
	echo ":: Updating Git submodule..."
	git submodule update --init --recursive || return 1
	local -r _URL='http://fftw.org/fftw-3.3.8.tar.gz'
	local -r _MD5='8aac833c943d8e90d51b697b27d4384d fftw-3.3.8.tar.gz'
	local -r _FILENAME='fftw-3.3.8.tar.gz'
	local -r _EXTRACTED_FILENAME='fftw-3.3.8'
	if [ -e "${_EXTRACTED_FILENAME}/README" ]; then
		echo ":: The file \"${_EXTRACTED_FILENAME}/README\" is aleady exists, so skipping download, verify and extract ${_FILENAME}."
	else
		if [ -e "${_FILENAME}" ]; then
			if [ -f "${_FILENAME}" ]; then
				echo ":: The file \"${_FILENAME}/\" is aleady exists, so skipping download it."
			else
				# Don't automatically delete it as it may be an important file.
				echo ":: \"${_FILENAME}\" is exists but it isn't a regular file, so please delete it and retry." >&2
				return 1
			fi
		else
			if command -v curl > /dev/null; then
				echo ":: Downloading \"${_URL}\" using cURL..."
				curl -LO ${_URL} || return 1
			else if command -v wget > /dev/null; then
				echo ":: Downloading \"${_URL}\" using Wget..."
				wget ${_URL} || return 1
				else
					echo "ERROR: cURL and Wget not found, please install at least one." >&2
					return 1
				fi
			fi
		fi
		echo ":: Checking MD5..."
		md5sum -c - <<< "${_MD5}" || { echo ":: md5sum failed, Please retry." >&2; echo ":: Deleting \"${_FILENAME}\"..."; rm -fv "${_FILENAME}"; return 1; }
		echo ":: Extracting \"${_FILENAME}\"..."
		tar xzkf "${_FILENAME}"
	fi
	return 0
}

main || exit 1

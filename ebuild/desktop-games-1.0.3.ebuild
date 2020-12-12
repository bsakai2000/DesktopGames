# Copyright 2020 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=7

MY_PN="DesktopGames"

DESCRIPTION="A simple collection of desktop games"
HOMEPAGE="https://github.com/bsakai2000/DesktopGames"
SRC_URI="https://github.com/bsakai2000/${MY_PN}/releases/download/v${PV}/${P}.tar.gz"

LICENSE="MIT"
SLOT="0"
KEYWORDS="~amd64"
IUSE=""

DEPEND=">=x11-libs/gtk+-3.0.0"
RDEPEND="${DEPEND}"
BDEPEND="dev-libs/glib"

.PHONY: release github-release

VERSION = $(shell LUA_PATH="./src/?.lua" lua -e 'print(require("luaflac.version")._VERSION)')

github-release:
	source $(HOME)/.github-token && github-release release \
	  --user jprjr \
	  --repo luaflac \
	  --tag v$(VERSION)
	source $(HOME)/.github-token && github-release upload \
	  --user jprjr \
	  --repo luaflac \
	  --tag v$(VERSION) \
	  --name luaflac-$(VERSION).tar.gz \
	  --file dist/luaflac-$(VERSION).tar.gz
	source $(HOME)/.github-token && github-release upload \
	  --user jprjr \
	  --repo luaflac \
	  --tag v$(VERSION) \
	  --name luaflac-$(VERSION).tar.xz \
	  --file dist/luaflac-$(VERSION).tar.xz

release:
	rm -rf dist/luaflac-$(VERSION)
	rm -rf dist/luaflac-$(VERSION).tar.gz
	rm -rf dist/luaflac-$(VERSION).tar.xz
	mkdir -p dist/luaflac-$(VERSION)/csrc
	rsync -a csrc/ dist/luaflac-$(VERSION)/csrc/
	rsync -a src/ dist/luaflac-$(VERSION)/src/
	rsync -a CMakeLists.txt dist/luaflac-$(VERSION)/CMakeLists.txt
	rsync -a LICENSE dist/luaflac-$(VERSION)/LICENSE
	rsync -a README.md dist/luaflac-$(VERSION)/README.md
	sed 's/@VERSION@/$(VERSION)/g' < rockspec/luaflac-template.rockspec > dist/luaflac-$(VERSION)/luaflac-$(VERSION)-1.rockspec
	cd dist && tar -c -f luaflac-$(VERSION).tar luaflac-$(VERSION)
	cd dist && gzip -k luaflac-$(VERSION).tar
	cd dist && xz luaflac-$(VERSION).tar


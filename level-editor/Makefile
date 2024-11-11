.phony: build test run build-web setup-web clean

install_trunk:
	cargo install --locked trunk
	
dependencies: install_trunk
	echo "export PATH=~/.cargo/bin:\$$PATH" >> ~/.zshrc && source ~/.zshrc

build: 
	cargo build

test:
	cargo test

run:
	cargo run

build-web:
	trunk build --release

clean:
	trunk clean
	cargo clean
#[cfg(not(target_arch = "wasm32"))]
use std::fs::File;
#[cfg(not(target_arch = "wasm32"))]
use std::io::prelude::*;

#[cfg(target_arch = "wasm32")]
use js_sys::wasm_bindgen::{JsCast, JsValue};

pub const ROWS: usize = 15; // 240 / 16
pub const COLUMNS: usize = 20; // 320 / 16
pub const SPRITE_NONE: char = ' ';

pub struct Level {
    pub tiles: Vec<Vec<char>>,
    // the width and height of the level, in terms of the number of "screens"
    pub width: usize,
    pub height: usize,
}

impl Level {
    pub fn rows(&self) -> usize {
        ROWS * self.height
    }

    pub fn columns(&self) -> usize {
        COLUMNS * self.width
    }

    fn build_tiles(&mut self) {
        let total_rows = self.rows();
        let total_cols = self.columns();

        let tiles = &mut self.tiles;
        tiles.clear();

        for _r in 0..total_rows {
            let mut row: Vec<char> = Vec::new();

            for _c in 0..total_cols {
                row.push(SPRITE_NONE);
            }

            tiles.push(row);
        }
    }
    pub fn new(width: usize, height: usize) -> Self {
        let mut level = Self {
            tiles: Vec::new(),
            width: width,
            height: height,
        };

        level.build_tiles();
        return level;
    }

    #[cfg(target_arch = "wasm32")]
    pub fn save(&mut self) -> std::io::Result<()> {
        let level_data = self
            .tiles
            .iter()
            .map(|row| row.iter().collect::<String>())
            .collect::<Vec<String>>()
            .join("\n");

        // convert the level data to a jsvalue
        let value = &JsValue::from_str(level_data.as_str());
        let value_arr = js_sys::Array::from_iter(std::iter::once(value));

        // create a blob from the jsvalue
        let blob = web_sys::Blob::new_with_str_sequence(&value_arr).expect("failed to create blob");

        // Create the download URL
        let url =
            web_sys::Url::create_object_url_with_blob(&blob).expect("failed to create object URL");

        // Create a temporary anchor element for downloading
        let window = web_sys::window().expect("failed to get window");
        let document = window.document().expect("failed to get document");
        let anchor = document
            .create_element("a")
            .expect("failed to create anchor");

        anchor
            .set_attribute("href", &url)
            .expect("failed to set href");
        anchor
            .set_attribute("download", "custom.level")
            .expect("failed to set download attribute");

        // Simulate click to trigger download
        anchor
            .dyn_ref::<web_sys::HtmlElement>()
            .expect("failed to convert to HtmlElement")
            .click();

        // Clean up
        web_sys::Url::revoke_object_url(&url).expect("failed to revoke object URL");

        Ok(())
    }

    #[cfg(not(target_arch = "wasm32"))]
    pub fn save(&mut self) -> std::io::Result<()> {
        let mut file = File::create("custom.level")?;
        let level_data = self
            .tiles
            .iter()
            .map(|row| row.iter().collect::<String>())
            .collect::<Vec<String>>()
            .join("\n");

        file.write_all(level_data.as_bytes())?;

        Ok(())
    }
}

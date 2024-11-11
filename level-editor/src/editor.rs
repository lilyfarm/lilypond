use eframe::egui;

use crate::level::Level;

pub struct Editor {
    current_tile: char,
    current_object: String,
    level: Level,
}

impl Default for Editor {
    fn default() -> Self {
        Self {
            current_tile: ' ',
            current_object: "none".to_string(),
            level: Level::new(1, 1),
        }
    }
}

impl Editor {
    fn tile_buttons(&mut self, ui: &mut egui::Ui) {
        for r in 0..self.level.rows() {
            ui.horizontal(|ui| {
                for c in 0..self.level.columns() {
                    let label =
                        egui::RichText::new(format!("{}", self.level.tiles[r][c])).monospace();

                    if ui.button(label).clicked() {
                        self.level.tiles[r][c] = self.current_tile;
                    }
                }
            });
        }
    }
    fn scrollable(&mut self, ctx: &egui::Context) {
        egui::CentralPanel::default().show(ctx, |ui| {
            let _scroll_area = egui::ScrollArea::both().show(ui, |ui| {
                self.tile_buttons(ui);
            });
        });
    }

    fn save_level(&mut self) {
        match self.level.save() {
            Ok(_) => {
                println!("level saved successfully");
            }

            Err(e) => {
                println!("could not save level: {}", e.to_string());
            }
        }
    }

    fn menu(&mut self, ctx: &egui::Context) {
        egui::TopBottomPanel::top("top_panel").show(ctx, |ui| {
            egui::menu::bar(ui, |ui| {
                ui.menu_button("File", |ui| {
                    if ui.button("Save Level").clicked() {
                        self.save_level();
                    }
                });
            });
        });
    }

    fn button_panel(&mut self, ctx: &egui::Context) {
        egui::SidePanel::left("sprite_panel")
            .default_width(300.0)
            .show(ctx, |ui| {
                ui.allocate_space(egui::vec2(300.0, 0.0));

                let label_text = format!(
                    "current object: {} ({})",
                    self.current_object, self.current_tile
                );

                ui.label(egui::RichText::new(label_text).monospace());

                for (tile_str, tile_chr) in crate::tiles::TILES {
                    let button_text = format!("{} | ({})", tile_str, tile_chr);
                    if ui
                        .button(egui::RichText::new(button_text).monospace())
                        .clicked()
                    {
                        self.current_object = tile_str.to_string();
                        self.current_tile = tile_chr;
                    }
                }

                ui.separator();

                ui.horizontal(|ui| {
                    ui.label("level width:");

                    if ui.button("-").clicked() {
                        if self.level.width == 1 {
                            return;
                        }

                        self.level.width -= 1;
                        self.level = Level::new(self.level.width, self.level.height);
                    }

                    ui.label(self.level.width.to_string());

                    if ui.button("+").clicked() {
                        if self.level.width == 6 {
                            return;
                        }

                        self.level.width += 1;
                        self.level = Level::new(self.level.width, self.level.height);
                    }
                });

                ui.horizontal(|ui| {
                    ui.label("level height:");

                    if ui.button("-").clicked() {
                        if self.level.height == 1 {
                            return;
                        }

                        self.level.height -= 1;
                        self.level = Level::new(self.level.width, self.level.height);
                    }

                    ui.label(self.level.height.to_string());

                    if ui.button("+").clicked() {
                        if self.level.height == 4 {
                            return;
                        }

                        self.level.height += 1;
                        self.level = Level::new(self.level.width, self.level.height);
                    }
                });
            });
    }
}

impl eframe::App for Editor {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        self.menu(ctx);
        self.button_panel(ctx);
        self.scrollable(ctx);
    }
}

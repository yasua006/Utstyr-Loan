(() => {
    'use strict';
    document.addEventListener('DOMContentLoaded', () => {
        class Skjema {
            main;
            skjema;
            constructor() {
                // få elementene
                this.main = document.querySelector('main');
                this.skjema = this.main && this.main.querySelector('.skjema');
                if (!this.main || !this.skjema) {
                    console.error("Hoved elementer mangler!");
                    return;
                }
                this.debounce_elementer();
            }
            debounce_elementer() {
                // lag en ny event listener for hver element med id
                this.skjema && this.skjema.childNodes.forEach(element => {
                    if (element instanceof HTMLElement && element.id) {
                        let debounce = false;
                        element.addEventListener('input', () => {
                            // debounce er på veien
                            if (debounce) {
                                return;
                            }
                            // start debounce
                            debounce = true;
                            // slutt debounce
                            setTimeout(() => {
                                debounce = false;
                            }, 500);
                        });
                    }
                });
            }
        }
        // kjør klassen
        new Skjema();
    });
})();

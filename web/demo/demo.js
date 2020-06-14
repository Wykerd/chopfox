/**
 *  This file is part of Chopfox.
 *
 *  Chopfox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Chopfox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with Chopfox.  If not, see <https://www.gnu.org/licenses/>.
 */

import "core-js/stable";
import "regenerator-runtime/runtime";

import cv from 'opencv'

import { simple_load_image, simple_processor_init_notext, SimpleComicData, simple_process_panels, simple_process_chop } from '../src/simple'

window.onload = async () => {
    window.cv = await cv();

    document.querySelector('input[type=file]').addEventListener('change', handle_img_change);
}

function handle_img_change (e) {
    const preview = document.querySelector('img');
    const file = e.target.files[0];
    const reader = new FileReader();
  
    reader.onloadend = async function () {
        const img = await simple_load_image(reader.result);
        window.cv.imshow('input_img', img);
        extract_frames(img);
    }
  
    if (file) {
        reader.readAsDataURL(file);
    } else {
        preview.src = "";
    }
}

function extract_frames(img) {
    const proc = simple_processor_init_notext();

    const data = new SimpleComicData();

    simple_process_panels(proc, img, data);

    simple_process_chop(proc, img, data);

    const cur_div = document.querySelector('section#output div');

    const div = document.createElement('div');

    cur_div.parentElement.replaceChild(div, cur_div);

    var msnry = new window.Masonry('section#output div', {
        columnWidth: 1,
        gutter: 20,
        horizontalOrder: true,
        transitionDuration: '0.8s'
    });

    data.frames.forEach(crop => {
        const canvas = document.createElement('canvas');
        canvas.classList.add('panel');
        canvas.style.marginBottom = '5px';
        div.appendChild(canvas);
        window.cv.imshow(canvas, crop);
        msnry.appended(canvas);
        msnry.layout();
    });
}

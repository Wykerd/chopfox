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
export function get_panels_rgb (mat, precision = 0.001) {
    if (mat.empty()) throw new Error('Cannot process empty Mat');
    
    const gray = new cv.Mat(), 
        lapl = new cv.Mat(), 
        thresh = new cv.Mat(), 
        dilated = new cv.Mat();
    
    cv.cvtColor(mat, gray, cv.COLOR_BGR2GRAY);

    cv.Laplacian(gray, lapl, cv.CV_8U);

    cv.threshold(lapl, thresh, 50, 255, cv.THRESH_BINARY);

    const dilation_kernel = cv.Mat.ones(3, 3, cv.CV_8U);
    cv.dilate(thresh, dilated, dilation_kernel);

    dilation_kernel.delete();

    const contours = new cv.MatVector();
    const hierarchy = new cv.Mat();

    const retVal = [];

    cv.findContours(dilated, contours, hierarchy, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE);

    for (let i = 0; i < contours.size(); i++) {
        const contour = contours.get(i);
        const approx = new cv.Mat();
        const epsilon = precision * cv.arcLength(contour, true);
        cv.approxPolyDP(contour, approx, epsilon, true);
        const area = cv.contourArea(contour);
        const min_area = (mat.size().height / 15.0) * (mat.size().width / 15.0);
        if (area > min_area) {
            const info = {};
            info.contour = approx;
            info.bounding_box = cv.boundingRect(approx);
            retVal.push(info);
        }
    }

    hierarchy.delete();
    contours.delete();

    dilated.delete();
    thresh.delete();
    lapl.delete();
    gray.delete();

    return retVal;
}

export function free_mat_vector (arr) {
    arr.forEach(i => {
        i.delete();
    });
}

export function crop_frames (src, panels) {
    if (src.empty()) throw new Error('Cannot process empty Mat');

    const retVal = [];

    for (const panel of panels) {
        const crop = src.roi(panel.bounding_box),
            mask = new cv.Mat(crop.rows, crop.cols, cv.CV_8UC1, new cv.Scalar(0)),
            isolated = new cv.Mat(),
            eroded = new cv.Mat();

        const contours = new cv.MatVector();
        contours.push_back(panel.contour);

        cv.fillPoly(mask, contours, new cv.Scalar(255), cv.LINE_8, 0, new cv.Point(-panel.bounding_box.x, -panel.bounding_box.y));

        const erode_kernel = cv.Mat.ones(3, 3, cv.CV_8U);
        cv.erode(mask, eroded, erode_kernel);
        erode_kernel.delete();

        crop.copyTo(isolated, eroded);

        crop.delete();
        mask.delete();
        eroded.delete();

        retVal.push(isolated);
    };

    return retVal;
}

export function draw_panel_bounds (panels, dst, color, thickness) {
    if (dst.empty()) throw new Error('Cannot draw to empty Mat');

    for (const panel of panels) {
        cv.rectangle(dst, panel.bounding_box, color, thickness);
    }
}
// Really simple config file for webpack
const webpack = require('webpack');
const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const CopyPlugin = require('copy-webpack-plugin');

module.exports = (env, argv) => {
    const isEnvProduction = argv.mode === 'production';

    const webConfig = {
        target: 'web',
        mode: isEnvProduction ? 'production' : 'development',
        entry: [path.resolve(__dirname, './demo/demo.js')],
        output: {
            path: path.resolve(__dirname, '../web_build'),
            filename: isEnvProduction ? '[name].[contenthash:8].js' : 'bundle.js',
            chunkFilename: isEnvProduction ? '[name].[contenthash:8].chunk.js' : '[name].chunk.js',
        },
        resolve: {
            extensions: [".js"]
        },
        node: {
            fs: 'empty',
            Buffer: false
        },
        externals: {
            opencv: 'cv'
        },
        module: {
            rules: [
                {
                    test: /\.m?js$/,
                    exclude: /(node_modules|bower_components)/,
                    use: {
                        loader: 'babel-loader',
                        options: {
                            "presets": [
                                [
                                    "@babel/preset-env",
                                    {
                                        "useBuiltIns": "usage",
                                        "corejs": 3
                                    }
                                ]
                            ]
                        }
                    }
                },
                {
                    test: /\.(png|jpe?g)$/i,
                    use: {
                        loader: 'file-loader',
                        options: {
                            outputPath: 'assets',
                            name: '[name].[contenthash:8].[ext]'
                        }
                    }
                }
            ]
        },
        plugins: [
            new CopyPlugin({
                patterns: [
                    { from: './cv/opencv_js.wasm', to: '.' },
                    { from: './cv/opencv_js.js', to: '.' },
                    { from: '../frozen_east_text_detection.pb', to: '.' }
                ]
            }),
            new webpack.ProgressPlugin(),
            new webpack.EnvironmentPlugin({
                NODE_ENV: isEnvProduction ? 'production' : 'development'
            }),
            new webpack.BannerPlugin('Copyright 2020 Daniel Wykerd\n'),
            new HtmlWebpackPlugin({
                template: path.resolve(__dirname, './demo/index.html'),
                filename: 'index.html'
            })
        ]
    }

    if (!isEnvProduction) {
        webConfig.devServer = {
            contentBase: path.resolve(__dirname, 'dist'),
            port: 9000,
            historyApiFallback: true
        };

        webConfig.devtool = 'inline-source-map';
    } 

    return webConfig;
}
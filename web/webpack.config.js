// Really simple config file for webpack
const webpack = require('webpack');
const path = require('path');
const CopyPlugin = require('copy-webpack-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');

module.exports = (env, argv) => {
    const isEnvProduction = argv.mode === 'production';

    const defaultConfig = {
        target: 'web',
        mode: isEnvProduction ? 'production' : 'development',
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
                }
            ]
        },
    }

    const libPlugins = [
        new CopyPlugin({
            patterns: [
                { from: './cv/opencv_js.wasm', to: '.' },
                { from: './cv/opencv_js.js', to: '.' }
            ]
        }),
        new webpack.ProgressPlugin(),
        new webpack.EnvironmentPlugin({
            NODE_ENV: isEnvProduction ? 'production' : 'development'
        }),
        new webpack.BannerPlugin('Copyright 2020 Daniel Wykerd\n'),
        new webpack.DefinePlugin({
            __COMMIT_HASH__: JSON.stringify(require('child_process')
                .execSync('git rev-parse --short HEAD')
                .toString().trim())
        })
    ]

    const demoConfig = {
        name: 'demo',
        entry: [path.resolve(__dirname, './demo/index.js')],
        output: {
            path: path.resolve(__dirname, '../docs'),
            filename: isEnvProduction ? '[name].[contenthash:8].js' : 'bundle.js',
            chunkFilename: isEnvProduction ? '[name].[contenthash:8].chunk.js' : '[name].chunk.js',
        },
        plugins: [
            ...libPlugins,
            new HtmlWebpackPlugin({
                template: path.resolve(__dirname, './demo/index.html'),
                filename: 'index.html'
            })
        ],
        ...defaultConfig
    }

    const webConfig = {
        name: 'lib',
        entry: [path.resolve(__dirname, './src/index.js')],
        output: {
            path: path.resolve(__dirname, '../web_build'),
            filename: isEnvProduction ? 'chopfox.min.js' : 'chopfox.dev.js',
            chunkFilename: isEnvProduction ? '[name].[contenthash:8].chunk.js' : '[name].chunk.js',
        },
        plugins: libPlugins,
        devtool: 'source-map',
        ...defaultConfig
    }

    if (!isEnvProduction) {
        demoConfig.devServer = {
            port: 9000,
            historyApiFallback: true
        };
        demoConfig.devtool = 'inline-source-map';
    }

    return [webConfig, demoConfig];
}
hunter_config(
    Boost VERSION "1.76.0-local"
    URL
    "https://osp-1257653870.cos.ap-guangzhou.myqcloud.com/FISCO-BCOS/FISCO-BCOS/deps/boost_1_76_0.tar.bz2
    https://downloads.sourceforge.net/project/boost/boost/1.76.0/source/boost_1_76_0.tar.bz2
    https://nchc.dl.sourceforge.net/project/boost/boost/1.76.0/boost_1_76_0.tar.bz2"
    SHA1
    8064156508312dde1d834fec3dca9b11006555b6
    CMAKE_ARGS
    # CONFIG_MACRO=BOOST_UUID_RANDOM_PROVIDER_FORCE_POSIX;BOOST_ALL_DYN_LINK
)

hunter_config(bcos-utilities VERSION 3.0.0-rc2-local
    URL https://${URL_BASE}/FISCO-BCOS/bcos-utilities/archive/fbe6d67de2e5d17f2ad35fd1730397239aa9373a.tar.gz
	SHA1 9cf973988b201d187e89aba69b2dd6d843a9612c
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON
)

hunter_config(bcos-boostssl
	VERSION 3.0.0-local
	URL https://github.com/LucasLi1024/bcos-boostssl/archive/29f0e40a11a7abe8729143e1c09c5305e3b45cf8.tar.gz
	SHA1 71ce2c8dac4087fd1399c2bf9ef004085e7f97e2
)



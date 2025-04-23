// textmorphing.js

document.addEventListener('DOMContentLoaded', () => {
    console.log("DOMContentLoaded: Initializing JS animation sequence.");

    const words = document.querySelectorAll('.initial-content .word');
    const wordCount = words.length;

    // CSSから速度を取得する（CSSカスタムプロパティを使うとより連携しやすい）
    // ここでは直接定義
    const speedSeconds = 20; // 通常速度（秒）
    const speedSlowSeconds = 40; // 遅い速度（秒）

    // アニメーションのステップ間の基本的な遅延時間（秒）
    // 元のCSSの計算に近い考え方 (速度は基準速度で計算)
    const baseIntervalSeconds = speedSeconds / (wordCount + 1);
    console.log(`Word count: ${wordCount}, Base interval: ${baseIntervalSeconds.toFixed(2)}s`);

    let currentIndex = 0;
    let cumulativeDelay = 0; // 次のアニメーション開始までの累積遅延時間 (ms)

    function animateNextWord() {
        if (wordCount === 0) return;

        // 現在の要素を取得 (ループさせる)
        const currentWord = words[currentIndex % wordCount];

        // 前回の animate クラスを削除 (念のため)
        words.forEach(w => w.classList.remove('animate'));

        // 現在の要素に animate クラスを追加してアニメーション開始
        console.log(`Animating word ${currentIndex % wordCount}: ${currentWord.textContent.trim()}`);
        currentWord.classList.add('animate');

        // この要素の速度を決定
        const isSlow = currentWord.classList.contains('slow');
        const currentDurationSeconds = isSlow ? speedSlowSeconds : speedSeconds;
        console.log(`  Duration: ${currentDurationSeconds}s ${isSlow ? '(Slow)' : '(Normal)'}`);

        // ★次のアニメーションを開始するタイミングを計算 (ms)
        //   単純に baseIntervalSeconds を使うか、
        //   現在の要素の実際の duration を考慮するか、調整が必要。
        //   ここでは baseIntervalSeconds を使うシンプルな方法を試す。
        const delayForNext = baseIntervalSeconds * 1000;

        // 次のインデックスへ
        currentIndex++;

        // 次のアニメーションをスケジュール
        setTimeout(animateNextWord, delayForNext);
    }

    // 最初のアニメーションを開始
    if (wordCount > 0) {
        // 少し間を置いて開始
        setTimeout(animateNextWord, 500);
    }

    // --- 時間によるテキスト変更ロジック ---
    // updateChangeNameText 関数と setInterval はそのまま残す
    function updateChangeNameText() {
        const targetElement = document.querySelector('.word.changeName');
        if (!targetElement) return; // 要素がなければ何もしない

        const now = new Date();
        const currentHour = now.getHours();
        let displayText = "Something"; // デフォルト

        if (currentHour >= 14 && currentHour < 16) { displayText = "1"; }
        else if (currentHour >= 16 && currentHour < 17) { displayText = "2"; }
        else if (currentHour >= 17 && currentHour < 18) { displayText = "3"; }

        if (targetElement.textContent !== displayText) {
            targetElement.textContent = displayText;
            // console.log("Updated text to:", displayText); // 必要ならログを有効化
        }
    }
    updateChangeNameText(); // 初期実行
    setInterval(updateChangeNameText, 60000); // 定期実行

});
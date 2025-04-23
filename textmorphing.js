// textmorphing.js

document.addEventListener('DOMContentLoaded', () => {
    console.log("DOMContentLoaded: Initializing pause logic using animationPlayState.");

    const words = Array.from(document.querySelectorAll('.initial-content .word'));
    const wordCount = words.length;
    const pauseDurationMs = 3000; // ポーズ時間 (ms)

    if (wordCount === 0) return;

    words.forEach((word, index) => {
        // 各単語の animationiteration イベントを監視
        word.addEventListener('animationiteration', (event) => {
            // console.log(`Iteration ended for word ${index}: ${word.textContent.trim()}`);

            // イベント発生元が .slow クラスを持っているか確認
            if (word.classList.contains('slow')) {
                // ★★★ 変更点: 現在の要素のアニメーションを一時停止 ★★★
                console.log(`Word ${index} is slow. Pausing its animation for ${pauseDurationMs}ms.`);
                word.style.animationPlayState = 'paused'; // アニメーションを一時停止

                // ポーズ時間後にアニメーションを再開するタイマーを設定
                setTimeout(() => {
                    console.log(`  Resuming animation for word ${index}.`);
                    word.style.animationPlayState = 'running'; // アニメーションを再開
                }, pauseDurationMs);
                 // ★★★ ここまで変更 ★★★
            }
        });
    });

    // --- 時間によるテキスト変更ロジック (変更なし) ---
    function updateChangeNameText() {
        const targetElement = document.querySelector('.word.changeName');
        if (!targetElement) return;
        const now = new Date();
        const currentHour = now.getHours();
        let displayText = "Something";
        if (currentHour >= 10 && currentHour < 15) { displayText = "   "; }
        else if (currentHour >= 15 && currentHour < 16) { displayText = "名古屋Orchest-Lab"; }
        else if (currentHour >= 16 && currentHour < 17) { displayText = "化ける身"; }
        else if (currentHour >= 17 && currentHour < 18) { displayText = "Rishao"; }
        if (targetElement.textContent !== displayText) {
            targetElement.textContent = displayText;
        }
    }
    updateChangeNameText();
    setInterval(updateChangeNameText, 60000);

});